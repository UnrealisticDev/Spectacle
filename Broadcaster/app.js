'use strict';
const env = require('dotenv');
const fs = require('fs');
const util = require('util');
const contentful = require('contentful-management');

env.config();
const {
	CONTENTFUL_MANAGEMENT_API_KEY,
	CONTENTFUL_SPACE_ID,
	CONTENTFUL_ENVIRONMENT,
	CONTENTFUL_TYPE,
} = process.env;

/**
 * Creates a Contentful client.
 */
const getContentfulClient = () => {
	return contentful.createClient({
		accessToken: CONTENTFUL_MANAGEMENT_API_KEY,
	});
};

/**
 * Loads parsed specifiers from dataset passed to us from Garganta.
 */
const loadParsedSpecifiers = async (filepath) => {
	const readFile = util.promisify(fs.readFile);
	try {
		return JSON.parse(await readFile(filepath));
	} catch (error) {
		return console.log('Failed to open or parse dataset file: ' + error);
	}
};

/**
 * Fetches existing specifiers using the Contentful API.
 * @returns existing specifiers
 */
const fetchExistingSpecifiers = async (client) => {
	try {
		const space = await client.getSpace(CONTENTFUL_SPACE_ID);
		const environment = await space.getEnvironment(CONTENTFUL_ENVIRONMENT);
		const specifiers = await environment.getEntries({
			content_type: CONTENTFUL_TYPE,
		});
		return specifiers;
	} catch (error) {
		console.error(error);
	}
};

/**
 * Creates a new entry that conforms to the Contentful API.
 */
const createEntry = (parsed) => {
	const { type, key, meta } = parsed;
	return {
		fields: {
			type: { 'en-US': type },
			key: { 'en-US': key },
			meta: { 'en-US': meta },
		},
	};
};

/**
 * Updates an existing entry in accordane with the Contentful API.
 */
const updateEntry = (existing, parsed) => {
	return {
		needsUpdate: false,
		update: null,
	};
};

/**
 * Compares existing and parsed specifiers and
 * determines what needs to be created/updated.
 * @param existingSpecifiers
 * @param parsedSpecifiers
 */
const calculateSpecifierDeltas = (existingSpecifiers, parsedSpecifiers) => {
	var deltas = {
		create: [],
		update: [],
	};

	for (const parsed of parsedSpecifiers.items) {
		const existing = existingSpecifiers.items.find((existing) => {
			const { type, key } = existing.fields;
			return parsed.type === type['en-US'] && parsed.key === key['en-US'];
		});

		if (!existing) {
			deltas.create.push(createEntry(parsed));
		} else {
			const { needsUpdate, update } = updateEntry(null, parsed);
			if (needsUpdate) {
				deltas.update.push(update);
			}
		}
	}

	return deltas;
};

/**
 * Simple timeout function used to space out API calls.
 */
function timeout(ms) {
	return new Promise((resolve) => setTimeout(resolve, ms));
}

/**
 * Pushes deltas to the Contentful database.
 * @param deltas
 */
const pushDeltas = async (client, deltas) => {
	for (const create of deltas.create) {
		client
			.getSpace(process.env.CONTENTFUL_SPACE_ID)
			.then((space) => space.getEnvironment(process.env.CONTENTFUL_ENVIRONMENT))
			.then((environment) =>
				environment.createEntry(process.env.CONTENTFUL_TYPE, create)
			);
		await timeout(1000);
	}
};

(async () => {
	try {
		const client = getContentfulClient();
		console.log('Opened client.');

        const parsedFilepath = process.argv[2];
		const parsedSpecifiers = await loadParsedSpecifiers(parsedFilepath);
		if (!parsedSpecifiers) {
			console.error('No parsed entries retrieved.');
			return;
		}

		console.log('Loaded %d parsed specifiers.', parsedSpecifiers.items.length);

		const existingSpecifiers = await fetchExistingSpecifiers(client);
		if (!existingSpecifiers) {
			console.error('No existing entries retrieved.');
			return;
		}

		console.log(
			'Loaded %d existing specifiers.',
			existingSpecifiers.items.length
		);

		const deltas = calculateSpecifierDeltas(
			existingSpecifiers,
			parsedSpecifiers
		);

		if (deltas.create.length === 0 && deltas.update.length === 0) {
			console.log('No delta between parsed and existing specifiers.');
			return;
		}

		console.log(
			'Identified %d new entries and %d entries to be updated.',
			deltas.create.length,
			deltas.update.length
		);

		await pushDeltas(client, deltas);
	} catch (error) {
		console.error(error);
	}
})();
