#include "SVGParser.h"
#include "SVGHelper.h"
#include "SVGHelperA2.h"
#include "LinkedListAPI.h"

#define SUCCESS 1
#define FAIL 0

/**
    The getNumber function is created for the file log panel
*/
int getNumber(char* componentType, char* filename){

    SVG* img = createValidSVG(filename);
    if (img == NULL) return FAIL;

    if (strcmp(componentType, "attr") == 0) return (getLength(img->otherAttributes));
    else if (strcmp(componentType, "rect") == 0) return (getLength(img->rectangles));
    else if (strcmp(componentType, "circ") == 0) return (getLength(img->circles));
    else if (strcmp(componentType, "path") == 0) return (getLength(img->paths));
    else if (strcmp(componentType, "group") == 0) return (getLength(img->groups));

    return FAIL;
}

/**
    The editAddImgFile function is created for the svg view panel
    in order to change an element in an svg file
*/
int editAddImgFile(char* componentType, int componentNumber, char* newName, char* newValue, char* filename){

    // 1. create new attribute value, svg image with filename
    Attribute* newAttribute = otherAttributes(newName, newValue);
    if (newAttribute == NULL) return FAIL;
    SVG* img = createValidSVG(filename);
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
