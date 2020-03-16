#include "hcml.h"

int main( int arg, char * argv [] ) {
    hcml_t _h = hcml_create();
    hcml_set_print_method(_h, "resp.write");
    
    // Parse the input file
    int _r = hcml_parse(_h, argv[1]);
    if ( _r == HCML_ERR_OK ) {
        printf("%s\n", hcml_get_output(_h) );
    } else {
        printf("%s\n", hcml_get_errstr(_h) );
    }

    hcml_destroy(_h);

    return 0;
}