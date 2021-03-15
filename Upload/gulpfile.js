const {src, dest, task, series} = require('gulp');
const exec = require('child_process').exec;

task('package', function(cb) {
    return exec('npm run package', cb);
});

task('transfer', function() {
    return src('dist/*.exe')
    .pipe(src('.env'))
    .pipe(dest(`../Binaries/x64/${process.argv[4]}/`));
});

task('deploy', series('package', 'transfer'));
