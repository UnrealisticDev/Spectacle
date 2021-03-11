'use strict';
const https = require('https');
const fs = require('fs');

console.log('Hello world!');
setTimeout(() => { console.log("Goodbye"); }, 5000);

console.log(process.argv[2]);
var databasePath = process.argv[2];
fs.readFile(databasePath, 'utf8', function (err, data) {
	if (err) {
		return console.log(err);
	}
	console.log(data);
});
