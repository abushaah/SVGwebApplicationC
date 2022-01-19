// Name: Hafaa Abushaaban [1146372]

#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "SVGHelper2.c"
#include "SVGParser.h"

int main(void){

    SVG* svg = createSVG("rect.svg");
    deleteSVG(svg);

    return 0;

}

SVG* createSVG(const char* filename){

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    SVG* svg = (SVG*) (malloc(sizeof(SVG)));

    int valid = 0; // 0 means false, 1 means true

    if (!filename) return NULL; // file is not given

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    doc = xmlReadFile(filename, NULL, 0); // parse the file and get the DOM

    if (doc == NULL){
        printf("Error: could not parse file %s in function createSVG\n", filename);
        return NULL;
    }

    root_element = xmlDocGetRootElement(doc); // root element node

/*
    valid = nameSpace(svg->namespace, root_element->ns->href, sizeof(svg->namespace), sizeof(root_element->ns->href)); // funciton to create namespace
    if (!valid){
        fprintf(stderr, "Tree support not compiled in\n");
        //deleteAll();
        return NULL;
    }
*/

    svg->rectangles = initializeList(&printFunc, &deleteFunc, &compareFunc);
    svg->circles = initializeList(&printFunc, &deleteFunc, &compareFunc);
    svg->paths = initializeList(&printFunc, &deleteFunc, &compareFunc);
    svg->groups = initializeList(&printFunc, &deleteFunc, &compareFunc);

    get_element_names(root_element, svg);

    xmlFreeDoc(doc); // free document
    xmlCleanupParser(); // free global variables allocated by parser

    return svg;

}

char* SVGToString(const SVG* img);

void deleteSVG(SVG* img){

    //free the lists
    free(img);

}
