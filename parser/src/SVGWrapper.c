#include "SVGParser.h"
#include "SVGHelper.h"
#include "SVGHelperA2.h"
#include "LinkedListAPI.h"
#include <strings.h>

bool validFile(char* filename){

    bool valid = true;

    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return false;

    valid = validateSVG(img, "uploads/svg.xsd");

    deleteSVG(img);
    return valid;
}

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

char* getTitle(char* filename){

    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return NULL;

    char* svgString = malloc(strlen(img->title) + 1);
    if (svgString == NULL) return NULL;

    strcpy(svgString, img->title);

    deleteSVG(img);
    return svgString;
}

char* getDescr(char* filename){

    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return NULL;

    char* svgString = malloc(strlen(img->description) + 1);
    if (svgString == NULL) return NULL;

    strcpy(svgString, img->description);

    deleteSVG(img);
    return svgString;

}

char* getRectsJSON(char* filename){

    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return NULL;

    char* rectsString = rectListToJSON(img->rectangles);
    if (rectsString == NULL) return NULL;

    deleteSVG(img);
    return rectsString;

}

char* getCircsJSON(char* filename){

    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return NULL;

    char* circsString = circListToJSON(img->circles);
    if (circsString == NULL) return NULL;

    deleteSVG(img);
    return circsString;

}

char* getPathsJSON(char* filename){

    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return NULL;

    char* pathsString = pathListToJSON(img->paths);
    if (pathsString == NULL) return NULL;

    deleteSVG(img);
    return pathsString;

}

char* getGroupsJSON(char* filename){

    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return NULL;

    char* groupsString = groupListToJSON(img->groups);
    if (groupsString == NULL) return NULL;

    deleteSVG(img);
    return groupsString;

}

char* getAttributesJSON(char* filename, char* componentType, int index){

    // 1. create the svg structure
    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return NULL;

    char* otherAttributesString;

    // 2. for the component type, different iterators
    if (strcasecmp(componentType, "Rectangle") == 0){
        // 3. get the index of the element that we want
        void* elem;
        ListIterator iter = createIterator(img->rectangles);
        int i = 0;
        while (((elem = nextElement(&iter)) != NULL) && (i < index)) ++i;
        Rectangle* rect = (Rectangle*) elem;

        // 4. get the list of other attributes
        otherAttributesString = attrListToJSON(rect->otherAttributes);
    }
    else if (strcasecmp(componentType, "Circle") == 0){
        void* elem;
        ListIterator iter = createIterator(img->circles);
        int i = 0;
        while (((elem = nextElement(&iter)) != NULL) && (i < index)) ++i;
        Circle* circ = (Circle*) elem;

        // 4. get the list of other attributes
        otherAttributesString = attrListToJSON(circ->otherAttributes);
    }
    else if (strcasecmp(componentType, "Path") == 0){
        // 3. get the index of the element that we want
        void* elem;
        ListIterator iter = createIterator(img->paths);
        int i = 0;
        while (((elem = nextElement(&iter)) != NULL) && (i < index)) ++i;
        Path* path = (Path*) elem;

        // 4. get the list of other attributes
        otherAttributesString = attrListToJSON(path->otherAttributes);
    }
    else if (strcasecmp(componentType, "Group") == 0){
        // 3. get the index of the element that we want
        void* elem;
        ListIterator iter = createIterator(img->groups);
        int i = 0;
        while (((elem = nextElement(&iter)) != NULL) && (i < index)) ++i;
        Group* group = (Group*) elem;

        // 4. get the list of other attributes
        otherAttributesString = attrListToJSON(group->otherAttributes);
    }

    deleteSVG(img);
    return otherAttributesString;

}

bool changeTitle(char* filename, char* newValue){

    if (newValue == NULL) return false;

    // 1. create svg
    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return false;

    // 2. validate the title. If valid, update the title
    if (validChar(newValue) == 0 || strlen(newValue) > 256){
        deleteSVG(img);
        return false;
    }
    strcpy(img->title, newValue);

    // 3. overwrite changes to file
    bool valid = writeSVG(img, filename);

    deleteSVG(img);
    return valid;
}

bool changeDescr(char* filename, char* newValue){

    if (newValue == NULL) return false;

    // 1. create svg
    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return false;

    // 2. validate the title. If valid, update the title
    if (validChar(newValue) == 0 || strlen(newValue) > 256){
        deleteSVG(img);
        return false;
    }
    strcpy(img->description, newValue);

    // 3. overwrite changes to file
    bool valid = writeSVG(img, filename);

    deleteSVG(img);
    return valid;
}

bool scaleRectangles(char* filename, float scaleValue){

    // 1. create svg
    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return false;

    // 2. get the rectangles and iterate through to scale
    List* rectangles = getRects(img);
    if (rectangles == NULL){
        deleteSVG(img);
        return false;
    }

    void* elem;
    ListIterator iter = createIterator(rectangles);
    while ((elem = nextElement(&iter)) != NULL){
        Rectangle* rect = (Rectangle*) elem;
        rect->x = rect->x * scaleValue;
        rect->y = rect->y * scaleValue;
    }

    freeList(rectangles);

    // 3. overwrite changes to file
    bool valid = writeSVG(img, filename);

    deleteSVG(img);
    return valid;

}

bool scaleCircles(char* filename, float scaleValue){

    // 1. create svg
    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL) return false;

    // 2. get the rectangles and iterate through to scale
    List* circles = getCircles(img);
    if (circles == NULL){
        deleteSVG(img);
        return false;
    }

    void* elem;
    ListIterator iter = createIterator(circles);
    while ((elem = nextElement(&iter)) != NULL){
        Circle* circ = (Circle*) elem;
        circ->cx = circ->cx * scaleValue;
        circ->cy = circ->cy * scaleValue;
    }

    freeList(circles);

    // 3. overwrite changes to file
    bool valid = writeSVG(img, filename);

    deleteSVG(img);
    return valid;

}

/**
    The setNewAttributes function is created for the svg view panel
    in order to change an element in an svg file
*/
bool setNewAttributes(char* filename, char* componentType, int componentNumber, char* newName, char* newValue){

    // 1. create new attribute value, and validate it
    bool valid = true;

    Attribute* newAttribute = otherAttributes(newName, newValue);
    if (newAttribute == NULL) return false;
    if (validAttrStruct(newAttribute) == false){
        deleteAttribute((void*)newAttribute);
        return false;
    }

    // 2. create svg based on file
    SVG* img = createValidSVG(filename, "uploads/svg.xsd");
    if (img == NULL){
        deleteAttribute((void*)newAttribute);
        return false;
    }

    // 3. identify component type
    elementType elem;
    if (strcasecmp(componentType, "Rectangle") == 0) elem = RECT;
    else if (strcasecmp(componentType, "Circle") == 0) elem = CIRC;
    else if (strcasecmp(componentType, "Path") == 0) elem = PATH;
    else if (strcasecmp(componentType, "Group") == 0) elem = GROUP;

    // 4. call c function to set or add attribute value in the file
    valid = setAttribute(img, elem, componentNumber, newAttribute);
    if (valid == false){
        deleteAttribute((void*)newAttribute);
        deleteSVG(img);
        return false;
    }

    // 5. overwrite changes to file
    valid = writeSVG(img, filename);

    // 6. memory management
    // 6. if the return vaue is false, will display error in the console
    deleteSVG(img);
    return valid;
}
