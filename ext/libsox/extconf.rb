require 'mkmf'

find_library("sox", "sox_format_init") or abort "can't find the sox libraries"
find_header("sox.h") or abort "can'f find the sox.h file"
create_makefile('libsox')
