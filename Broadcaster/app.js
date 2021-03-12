'use strict';
const fs = require('fs');

/** 
 * Ensure that parsed dataset has been passed to us from Garganta. 
 * Otherwise, what are we even here for?! 
 */
const parsedDatasetPath = process.argv[2];
if ( !parsedDatasetPath ) {
    return console.error("No parsed dataset file provided.")
}
console.log("Received dataset file: " + parsedDatasetPath);
console.

console.log("Loading dataset.");
var parsedDataset;
try { 
    parsedDataset = JSON.parse(fs.readFileSync(parsedDatasetPath, 'utf8'));
    if ( !parsedDataset || !parsedDataset.data ) {
        return console.error("Provided dataset was empty.");
    }
} catch (error) {
    return console.log("Failed to open or parse dataset file: " + error);
}
console.log("Loaded dataset successfully.");
console.log("Dataset contains %d entries", parsedDataset.data.length);

/**
 * Request all current specifier entries from Contentful
 * to be compared against and operated on.
 * (Down the line could potentially pull only entries that match dataset)
 */

// const client = contentful.createClient({
//     space: '4z1i7rtd5bsh',
//     environment: 'master',
//     accessToken: 'Xeqkswi8vvM5us2jEa7ESxU4MyAqyZQj5EHQ1SyzTe8'
// });

// client.getEntries({
//     content_type: 'unrealSpecifier'
// }).then((response) => console.log(response.items)).catch(console.error);

setTimeout(() => { console.log('Timed out.') }, 20000);
