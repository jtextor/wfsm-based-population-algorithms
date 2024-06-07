
png_out <- function( file, pointsize=8, ... ){
        if( capabilities()['aqua'] ){
                # preferred device type with configurable fonts
                quartz( type="png", file=file, pointsize=pointsize, ...)
                par( family="sans" )

        } else {
                # fallback device type with default Helvetica font
                png( file, pointsize=pointsize, ...)
        }
}

pdf_out <- function( file, pointsize=8, ... ){
        if( capabilities()['aqua'] ){
                # preferred device type with configurable fonts
                quartz( type="pdf", file=file, pointsize=pointsize, ...)
                par( family="sans" )

        } else {
                # fallback device type with default Helvetica font
                pdf( file, pointsize=pointsize, ...)
        }
}

quartzFonts(sans = quartzFont(c("Helvetica Neue Light","Helvetica Neue",
        "Helvetica Neue Oblique","Helvetica Neue Oblique")))


default.mgp <- c(2.3,1,0)
