// Name: Haifaa Abushaaban[1146372]
/*
    Sample code "tree2.c" used for understanding how to create a document and nodes
    for an xml tree from: http://www.xmlsoft.org/examples/tree2.c
    Sample code "xmlvalidation.c" used for understanding how to validate xmlDoc against
    an XSD file from: http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include <libxml/xmlschemastypes.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "LinkedListAPI.h"
#include "SVGParser.h"
#include "SVGHelper.h"
#include "SVGHelperA2.h"

#define LIBXML_SCHEMAS_ENABLED

/*
    createValidSVG first creates an xml tree, then validates the svg file against the schema file,
    then calls create svg for the svg file to create an svg struct if the file is valid
*/
SVG* createValidSVG(const char* fileName, const char* schemaFile){

    if (fileName == NULL || schemaFile == NULL) return NULL;

    // 1. parse the file and get the DOM
    xmlDocPtr doc = NULL;
    doc = xmlReadFile(fileName, NULL, 0);
    if (doc == NULL){
        xmlCleanupParser(); // free global variables allocated by parser in the validateFileSVG() function
        return NULL;
    }

    // 2. validates the svg against the schema file
    bool valid = validateFileSVG(doc, schemaFile);
    if (valid == false){
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    xmlFreeDoc(doc);
    xmlCleanupParser(); // free global variables allocated by parser in the validateFileSVG() function

    // 3. create the svg, that is valid?
    SVG* svg = createSVG(fileName);
    if (svg == NULL){
        return NULL;
    }

    return svg;
}

/*
    writes an svg struct to a file in the svg format
    assumes that the struct has been validated!
*/
bool writeSVG(const SVG* img, const char* fileName){

    if (img == NULL || fileName == NULL) return false;

    // 1. create the xml doc from the svg
    xmlDocPtr doc = createXMLFromStruct(img);
    if (doc == NULL){
        xmlCleanupParser();
        return false;
    }

    // 2. save contents to a file
    xmlSaveFormatFileEnc(fileName, doc, "UTF-8", 1);

    // 3. free the document
    xmlFreeDoc(doc);
    xmlCleanupParser(); // free global variables allocated by parser in the createXMLFromStruct() function

    return true;
}

/*
    validates every struct and list
*/
bool validateSVG(const SVG* img, const char* schemaFile){

    if (img == NULL || schemaFile == NULL) return false;

    // 1. Convert the svg to an xml doc using write to file
    xmlDocPtr doc = createXMLFromStruct(img);
    if (doc == NULL){
        xmlCleanupParser();
        return false;
    }

    xmlCleanupParser(); // free global variables allocated by parser in the createXMLFromStruct() function

    // 2. Validate the xml doc against the schema similar to the create valid svg function
    bool valid = validateFileSVG(doc, schemaFile);
    if (valid == false){
        xmlFreeDoc(doc);
        xmlCleanupParser(); // free global variables allocated by parser in the validateFileSVG() function
        return false;
    }

    xmlFreeDoc(doc);
    xmlCleanupParser(); // free global variables allocated by parser in the validateFileSVG() function

    // 3. Validate the svg struct against the svgparser.h specifications
    valid = validSVGStruct(img);
    if (valid == false) return false;

    return true;
}

bool setAttribute(SVG* img, elementType elemType, int elemIndex, Attribute* newAttribute){

    if (img == NULL || newAttribute == NULL) return false;

    // 1. valid attribute strings
    if ((validChar(newAttribute->name) == 0) || (strcmp(newAttribute->name, "") == 0) || (validChar(newAttribute->value) == 0) || (strcmp(newAttribute->value, "") == 0)) return false;

    // 2. check the type
    if (elemType == SVG_IMG){
        bool valid = changeValueInAttr(img->otherAttributes, newAttribute);
        if (valid == false) return false;
    }
    else if (elemType == RECT){
        // 3. check if index is out of bounds
        if (getLength(img->rectangles) <= elemIndex) return false;
        // 4. separate function for changing value in order to check for validity of the new attribute
        //    the functions are in charge of freeing the newAttribute attribute depending on whether it was appended or not
        bool valid = changeValueInRect(img->rectangles, elemIndex, newAttribute);
        if (valid == false) return false;

    }
    else if (elemType == CIRC){
        if (getLength(img->circles) <= elemIndex) return false;
        bool valid = changeValueInCirc(img->circles, elemIndex, newAttribute);
        if (valid == false) return false;
    }
    else if (elemType == PATH){
        if (getLength(img->paths) <= elemIndex) return false;
        bool valid = changeValueInPath(img->paths, elemIndex, newAttribute);
        if (valid == false) return false;
    }
    else if (elemType == GROUP){
        if (getLength(img->groups) <= elemIndex) return false;
        bool valid = changeValueInGroup(img->groups, elemIndex, newAttribute);
        if (valid == false) return false;
    }
    else{ // not the appropriate element
        return false;
    }

    return true;

}

void addComponent(SVG* img, elementType type, void* newElement){

    if (img == NULL || newElement == NULL) return;

    // 1. determine component type
    if (type == RECT){
        if (img->rectangles == NULL) return;
        Rectangle* newRect = (Rectangle*) newElement;
        // 2. check if component is valid
        bool valid = validRectStruct(newRect);
        if (valid == false){
            deleteRectangle(newRect);
            return;
        }
        // 3. add comopnent to the end of the list
        insertBack(img->rectangles, (void*)newRect);
    }
    else if (type == CIRC){
        if (img->circles == NULL) return;
        Circle* newCirc = (Circle*) newElement;
        bool valid = validCircStruct(newCirc);
        if (valid == false){
            deleteCircle(newCirc);
            return;
        }
        insertBack(img->circles, (void*)newCirc);
    }
    else if (type == PATH){
        if (img->paths == NULL) return;
        Path* newPath = (Path*) newElement;
        bool valid = validPathStruct(newPath);
        if (valid == false){
            deletePath(newPath);
            return;
        }
        insertBack(img->paths, (void*)newPath);
    }

}
