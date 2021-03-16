'use strict';
const env = require('dotenv');
const fs = require('fs');
const path = require('path');
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
 * Fetches existing specifier entries from Contentful.
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
		console.error('Failed to fetch existing specifiers: ' + error);
	}
};

/**
 * Loads parsed result from file.
 */
const loadParsedResult = async (file) => {
	try {
		return JSON.parse(await fs.promises.readFile(file));
	} catch (error) {
		return console.log('Failed to open or parse result file: ' + error);
	}
};

/**
 * Creates a specifier entry that conforms to the Contentful API from a result.
 */
const createSpecifierFromParsedResult = (result, version) => {
	const { type, key, meta, occ } = result;
	return {
		fields: {
			type: { 'en-US': type },
			key: { 'en-US': key },
			meta: { 'en-US': meta },
			occ: {
				'en-US': {
					versions: [
						{
							version: version,
							items: occ,
						},
					],
				},
			},
		},
	};
};

/**
 * Calculates the delta between a parsed specifier and a
 * set of existing specifiers.
 */
const calculateSpecifierDelta = (parsedSpecifier, existingSpecifiers) => {
	const existingSpecifier = existingSpecifiers.items.find(
		(existingSpecifier) => {
			return (
				parsedSpecifier.fields.type['en-US'] ===
					existingSpecifier.fields.type['en-US'] &&
				parsedSpecifier.fields.key['en-US'] ===
					existingSpecifier.fields.key['en-US']
			);
		}
	);

	if (existingSpecifier) {
		const { occ } = existingSpecifier.fields;
		if (!occ['en-US'].versions.some(({ version }) => version == 'release')) {
			var updatedSpecifier = existingSpecifier;
			updatedSpecifier.fields.occ['en-US'].versions.push(
				parsedSpecifier.fields.occ['en-US'].versions[0]
			);
			return {
				type: 'update',
				entry: updatedSpecifier,
			};
		} else {
			return null;
		}
	} else {
		return {
			type: 'create',
			entry: parsedSpecifier,
		};
	}
};

/**
 * Simple timeout function used to space out API calls.
 */
function timeout(ms) {
	return new Promise((resolve) => setTimeout(resolve, ms));
}

/**
 * Pushes deltas to the Contentful database.
 */
const pushEntryDeltas = async (client, deltas) => {
	for (const delta of deltas) {
		client
			.getSpace(process.env.CONTENTFUL_SPACE_ID)
			.then((space) => space.getEnvironment(process.env.CONTENTFUL_ENVIRONMENT))
			.then((environment) => {
				if (delta.type == 'create') {
					environment.createEntry(process.env.CONTENTFUL_TYPE, delta.entry);
				} else if (delta.type == 'update') {
					delta.entry.update();
				}
			});
		await timeout(1000);
	}
};

(async function () {
	try {
		const client = getContentfulClient();
		console.log('Opened client.');

		const existingSpecifiers = await fetchExistingSpecifiers(client);
		if (!existingSpecifiers) {
			console.error('No existing entries retrieved.');
			return;
		}

		console.log(
			'Loaded %d existing specifiers.',
			existingSpecifiers.items.length
		);

		const parsedResultDirectory = process.argv[2]
			? process.argv[2]
			: 'C:\\Users\\Mustafa\\AppData\\Local\\Temp\\Spectacle\\Results\\';
		if (!(await fs.promises.stat(parsedResultDirectory)).isDirectory()) {
			console.error('Provided result directory does not exist.');
		}

		const files = await fs.promises.readdir(parsedResultDirectory);
		if (files.length === 0) {
			console.error('No results to parse in result directory.');
			return;
		}

		const deltas = [];
		for (const file of files) {
			console.log(file);
			const parsedSpecifier = createSpecifierFromParsedResult(
				await loadParsedResult(path.resolve(parsedResultDirectory, file)),
				process.argv[3] ? process.argv[3] : 'release'
			);
			console.log(parsedSpecifier);
			const delta = calculateSpecifierDelta(
				parsedSpecifier,
				existingSpecifiers
			);
			if (delta) {
				deltas.push(delta);
			}
		}

		if (deltas.length === 0) {
			console.log('No delta between parsed and existing specifiers.');
			return;
		}

		var create = 0;
		var update = 0;
		deltas.forEach((d) => {
			if (d.type === 'create') {
				++create;
			} else {
				++update;
			}
		});
		console.log('Identified %d new entries.', create);
		console.log('Identified %d update entries.', update);

		await pushEntryDeltas(client, deltas);
	} catch (error) {
		console.error(error);
	}
})();
