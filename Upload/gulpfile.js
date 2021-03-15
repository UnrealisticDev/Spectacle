const {src, dest, task} = require('gulp');

task('package', function() {
    return src('upload.js')
    .pipe(src('.env'))
    .pipe(dest('../Binaries/x64/Debug/'))
});
