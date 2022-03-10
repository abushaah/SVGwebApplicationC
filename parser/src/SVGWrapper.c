#include "SVGParser.h"
#include "SVGHelper.h"
#include "SVGHelperA2.h"
#include "LinkedListAPI.h"
#include <strings.h>

#define SUCCESS 1
#define FAIL 0

/**
    The getNumber function is created for the file log panel
*/
char* getNumber(char* filename){

    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return NULL;

    char* numbers = SVGtoJSON(img);

    deleteSVG(img);
    return numbers;
}

bool validFile (char* filename){

    SVG* svg = createValidSVG(filename, "uploads/svg.xsd");
    if (svg == NULL){
        return false;
    }

    bool valid = validateSVG(svg, "uploads/svg.xsd");
    if (valid == false){
        deleteSVG(svg);
        return false;
    }

    deleteSVG(svg);
    return true;

}

/**
    The editAddImgFile function is created for the svg view panel
    in order to change an element in an svg file
*/
int editAddImgFile(char* componentType, int componentNumber, char* newName, char* newValue, char* filename){

    // 1. create new attribute value, svg image with filename
    Attribute* newAttribute = otherAttributes(newName, newValue);
    if (newAttribute == NULL) return FAIL;
    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return FAIL;

    elementType elem;
    if (strcasecmp(componentType, "Attribute") == 0) elem = SVG_IMG; // from string on the page
    else if (strcasecmp(componentType, "Rectangle") == 0) elem = RECT;
    else if (strcasecmp(componentType, "Circle") == 0) elem = CIRC;
    else if (strcasecmp(componentType, "Path") == 0) elem = PATH;
    else if (strcasecmp(componentType, "Group") == 0) elem = GROUP;

    // 2. call c function to set attribute value in the file
    // 3. if the return vaue is false, will display error in the console
    bool valid = setAttribute(img, elem, componentNumber, newAttribute);
    if (valid == false) return FAIL;

    // d. memory management
    deleteSVG(img);

    return SUCCESS;
}
