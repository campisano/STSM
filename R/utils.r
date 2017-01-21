utils = new.env(hash=TRUE, parent=emptyenv());



utils$verbose = function() {
    options(warn=2, keep.source=TRUE);
}



# defining functions
utils$loadLibs = function(lib_names) {
    repos = "http://cran.r-project.org";
    lib = paste(Sys.getenv("HOME"), "R", "library", sep="/");
    dir.create(lib, showWarnings=FALSE, recursive=TRUE, mode="2755");
    .libPaths(c(lib, .libPaths()));

    for(lib_name in lib_names) {
        if (!require(lib_name, character.only=TRUE, quietly=TRUE)) {
            install.packages(lib_name, repos=repos, lib=lib,
                             quiet=FALSE, dependencies = TRUE);
            library(lib_name, character.only=TRUE, quietly=TRUE);
        }
    }

    return(lib_names);
}



utils$dev_open_file = function(file_name, width=480, height=480, scale=1) {
    ext = strsplit(file_name, "\\.")[[1]][[-1]];

    dpi = 72
    pointsize = 12 * dpi / 72  # do not change

    if(ext == "svg") {
        svg(file_name, bg="transparent", antialias="none",
            pointsize=pointsize,
            width=(width / dpi) * scale, height=(height / dpi) * scale);
    }
    if(ext == "png") {
        png(file_name, bg="transparent", antialias="none",
            pointsize=pointsize, units="in", res=dpi,
            width=(width / dpi) * scale, height=(height / dpi) * scale);
    }
}
