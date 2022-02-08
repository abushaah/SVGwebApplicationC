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
#include "SVGHelperA2.h"

#define LIBXML_SCHEMAS_ENABLED

/*
    createValidSVG first validates the svg file against the schema file,
    then calls create svg for the svg file to create an svg struct
*/
SVG* createValidSVG(const char* fileName, const char* schemaFile){

    if (fileName == NULL || schemaFile == NULL) return false;
    bool valid = validateFileSVG(fileName, schemaFile);
    if (valid == false) return NULL;

    // negative values in the width/height will not be considered invalid,
    // so funcion "validateSVG" will test the svg against the constraints specified in SVGParser.h
    SVG* svg = createSVG(fileName);
    if (svg == NULL) return NULL;

    return svg;
}

/*
    writes an svg struct to a file in the svg format
    assumes that the struct has been validated
*/
bool writeSVG(const SVG* img, const char* fileName){

    if (img == NULL || fileName == NULL) return false;

    FILE* file = fopen(fileName, "w");
    if (file == NULL){
        return false;
    }

    xmlDocPtr doc = NULL; // document pointer
    xmlNodePtr root_node = NULL; // root of tree

    LIBXML_TEST_VERSION;

    // 1. convert the SVG to an XML by creating a new document, a node and set it as a root node
    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "svg");
    xmlDocSetRootElement(doc, root_node);

    // 2. set the namespace, title, and description using the xmlNewText
    xmlNewProp(root_node, BAD_CAST "xmlns", BAD_CAST img->namespace);

    if (strlen(img->title) != 0){
        xmlNodePtr titleNode = xmlNewNode(NULL, BAD_CAST "title");
        xmlNodePtr titleNodeTxt = xmlNewText(BAD_CAST img->title);
        xmlAddChild(titleNode, titleNodeTxt);
        xmlAddChild(root_node, titleNode);
    }
    if (strlen(img->description) != 0){
        xmlNodePtr descNode = xmlNewNode(NULL, BAD_CAST "desc");
        xmlNodePtr descNodeTxt = xmlNewText(BAD_CAST img->description);
        xmlAddChild(descNode, descNodeTxt);
        xmlAddChild(root_node, descNode);
    }

    /* 3.
       calling functions that will loop through list (other attributes, rect, circ, path, groups),
       and adds items to the parent, root_node svg
       these functions will be used for both the svg children and the group children
    */
    addAttrListToParentNode(img->otherAttributes, &root_node); // ask: first or last?
    addRectListToParentNode(img->rectangles, &root_node);
    addCircListToParentNode(img->circles, &root_node);
    addPathListToParentNode(img->paths, &root_node);
    addGroupListToParentNode(img->groups, &root_node);

    // 4. save contents to a file
    xmlSaveFormatFileEnc(fileName, doc, "UTF-8", 1);
    fclose(file);

    // 5. free the document, global variables that may have been allocated by the parser
    xmlFreeDoc(doc);
    xmlCleanupParser();

    // debug memory for regression tests
    xmlMemoryDump();

    return true;
}

//bool validateSVG(const SVG* img, const char* schemaFile);
/*
void validateXMLTree(){
}
*/
