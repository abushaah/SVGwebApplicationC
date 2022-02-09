// Name: Haifaa Abushaaban[1146372]
/*
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
    given an xml file name and a schema file,
    this function will parse the schema file,
    parse the svg tree,
    and if both files are valid,
    will validate the file against the schema file
    if any function is not valid or the file validation is not successful, will return false
    if all functions succeed and the file validation is successful, will return true
*/
bool validateFileSVG(const char* fileName, const char* schemaFile){

    if (fileName == NULL || schemaFile == NULL) return false;

    xmlDocPtr doc;
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr pCtxt;

    xmlLineNumbersDefault(1);

    // schema parser for the schema file
    pCtxt = xmlSchemaNewParserCtxt(schemaFile);
    if (pCtxt == NULL) return false; // error creating context

    xmlSchemaSetParserErrors(pCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);

    schema = xmlSchemaParse(pCtxt);
    if (schema == NULL) return false; // error parsing schema file

    xmlSchemaFreeParserCtxt(pCtxt);

    // tree parsing for the svg file
    doc = xmlReadFile(fileName, NULL, 0);
    if (doc == NULL) return false; // error parsing svg file

    xmlSchemaValidCtxtPtr vCtxt;
    int ret;

    // validate the schema against the svg tree
    vCtxt = xmlSchemaNewValidCtxt(schema);
    if (vCtxt == NULL) return false; // error creating context

    xmlSchemaSetValidErrors(vCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    ret = xmlSchemaValidateDoc(vCtxt, doc);

    // xmlSchemaValidateDoc returns 0 if the document is schemas valid, any other number indicates fail
    if (ret != 0){
        xmlSchemaFreeValidCtxt(vCtxt);
        xmlFreeDoc(doc);
        if (schema != NULL) xmlSchemaFree(schema);
        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();
        return false;
    }

    // ret == 0, the file is valid, can create a valid svg
    xmlSchemaFreeValidCtxt(vCtxt);
    xmlFreeDoc(doc);
    // free the resource
    if (schema != NULL) xmlSchemaFree(schema);

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();

    return true;

}

/*
    this function is for adding items in a list to a parent when:
    - other attributes in an svg
    - other attributes in a shape struct such as circle rect and path
    - other attributes in a group
    - parent can be either SVG, group, or shape with other attirbute list
*/
void addAttrListToParentNode(List* list, xmlNodePtr* parent){

    if (list == NULL || *parent == NULL) return;

    void* elem;
    ListIterator iter = createIterator(list); // traverse through the other attributes
    while ((elem = nextElement(&iter)) != NULL){
        Attribute* attr = (Attribute*) elem;
        // parent node, name="value"
        xmlNewProp(*parent, BAD_CAST attr->name, BAD_CAST attr->value);
    }

}

/*
    given a list of rectangles, this function creates a new node for each rectangle struct,
    adds the properties/attributes in the node,
    and adds that node to the parent node given
*/
void addRectListToParentNode(List* rectList, xmlNodePtr* parent){

    if (rectList == NULL || *parent == NULL) return;

    // 1. iterate through the struct list
    void* elem;
    ListIterator iter = createIterator(rectList); // traverse through the rectangle
    while ((elem = nextElement(&iter)) != NULL){

        Rectangle* rect = (Rectangle*) elem;

        // 2. rectangle node, adding it to the parent (svg or group)
        xmlNodePtr node = xmlNewChild(*parent, NULL, BAD_CAST "rect", NULL);

        // 3. adding contents to the rectangle
        char* x = unitsWithNumber(rect->x, rect->units);
        char* y = unitsWithNumber(rect->y, rect->units);
        char* width = unitsWithNumber(rect->width, rect->units);
        char* height = unitsWithNumber(rect->height, rect->units);

        xmlNewProp(node, BAD_CAST "x", BAD_CAST x);
        xmlNewProp(node, BAD_CAST "y", BAD_CAST y);
        xmlNewProp(node, BAD_CAST "width", BAD_CAST width);
        xmlNewProp(node, BAD_CAST "height", BAD_CAST height);

        // 4. adding other attributes to the rectangles
        addAttrListToParentNode(rect->otherAttributes, &node);

        free(x);
        free(y);
        free(width);
        free(height);
    }
}

/*
    given a list of circles, this function creates a new node for each circle struct,
    adds the properties/attributes in the node,
    and adds that node to the parent node given
*/
void addCircListToParentNode(List* circList, xmlNodePtr* parent){

    if (circList == NULL || *parent == NULL) return;

    // 1. iterate through the struct list
    void* elem;
    ListIterator iter = createIterator(circList); // traverse through the rectangle
    while ((elem = nextElement(&iter)) != NULL){

        Circle* circ = (Circle*) elem;

        // 2. circle node, adding it to the parent (svg or group)
        xmlNodePtr node = xmlNewChild(*parent, NULL, BAD_CAST "circle", NULL);

        // 3. adding contents to the circle
        char* cx = unitsWithNumber(circ->cx, circ->units);
        char* cy = unitsWithNumber(circ->cy, circ->units);
        char* r = unitsWithNumber(circ->r, circ->units);

        xmlNewProp(node, BAD_CAST "cx", BAD_CAST cx);
        xmlNewProp(node, BAD_CAST "cy", BAD_CAST cy);
        xmlNewProp(node, BAD_CAST "r", BAD_CAST r);

        // 4. adding other attributes to the circle
        addAttrListToParentNode(circ->otherAttributes, &node);

        free(cx);
        free(cy);
        free(r);
    }
}

/*
    given a list of paths, this function creates a new node for each path struct,
    adds the properties/attributes in the node,
    and adds that node to the parent node given
*/
void addPathListToParentNode(List* pathList, xmlNodePtr* parent){

    if (pathList == NULL || *parent == NULL) return;

    // 1. iterate through the struct list
    void* elem;
    ListIterator iter = createIterator(pathList); // traverse through the rectangle
    while ((elem = nextElement(&iter)) != NULL){

        Path* path = (Path*) elem;

        // 2. path node, adding it to the parent (svg or group)
        xmlNodePtr node = xmlNewChild(*parent, NULL, BAD_CAST "path", NULL);

        // 3. adding contents to the path
        xmlNewProp(node, BAD_CAST "d", BAD_CAST path->data);

        // 4. adding other attributes to the path
        addAttrListToParentNode(path->otherAttributes, &node);
    }
}

/*
    this function is for adding items from a group struct
    it is recursive, as it calls itself when it reaches an inner group list
*/
void addGroupListToParentNode(List* groupList, xmlNodePtr* parent){

    if (groupList == NULL || *parent == NULL) return; // base case

    // 1. iterate through the struct list
    void* elem;
    ListIterator iter = createIterator(groupList); // traverse through the group
    while ((elem = nextElement(&iter)) != NULL){

        Group* group = (Group*) elem;

        // 1. create the node for the group
        xmlNodePtr node = xmlNewChild(*parent, NULL, BAD_CAST "g", NULL);

        // 2. call the other attribute rect, circle, path, functions with the
        // node as the parent in this case parent is a group node
        addAttrListToParentNode(group->otherAttributes, &node); // ask: first or last?
        addRectListToParentNode(group->rectangles, &node);
        addCircListToParentNode(group->circles, &node);
        addPathListToParentNode(group->paths, &node);
        addGroupListToParentNode(group->groups, &node);
    }

}

/*
    used for circles and rectangles, this funciton will return a string with the
    number and units concatinated
    units should not be null and should be initialized, according to the struct
    definition in the createSVG function in A1
    caller must free string
*/
char* unitsWithNumber(float number, char units[]){

    char* numWithUnits = numWithUnits = malloc(50 + (strlen(units)) + 1); // max length of float, units, and \0

    sprintf(numWithUnits, "%f", number);

    if (strlen(units) > 0){
        strcat(numWithUnits, units);
    }

    return numWithUnits;

}

/*
    validSVGStruct helper function will take an svg and check for validity:
    - Namespace must be present and cannot be empty
    - Title, description must be initialized and must not be empty
    - Lists must not be NULL
    - Loops through items in lists and validates each struct in those lists
*/
bool validSVGStruct(const SVG* svg){

    if (svg == NULL) return false;

    // 1. initialized values?
    if ((svg->namespace == NULL) ||
       (svg->title == NULL) ||
       (svg->description == NULL) ||
       (svg->rectangles == NULL) ||
       (svg->circles == NULL) ||
       (svg->paths == NULL) ||
       (svg->groups == NULL) ||
       (svg->otherAttributes == NULL)) return false;

    // 2. empty strings?
    if (validChar((char*)svg->namespace) == 0) return false;
    if (emptyString((char*)svg->namespace) == 0) return false;

    // 3. if not empty, are the list contents valid?
    if ((isListEmpty(svg->otherAttributes) == 0) && (validAttrListStruct(svg->otherAttributes) == false)) return false;
    if ((isListEmpty(svg->rectangles) == 0) && (validRectListStruct(svg->rectangles) == false)) return false;
    if ((isListEmpty(svg->circles) == 0) && (validCircListStruct(svg->circles) == false)) return false;
    if ((isListEmpty(svg->paths) == 0) && (validPathListStruct(svg->paths) == false)) return false;
    if ((isListEmpty(svg->groups) == 0) && (validGroupListStruct(svg->groups) == false)) return false;

    // 4. success
    return true;

}

// validates single attribute structure
bool validAttrStruct(Attribute* attr){
    if (attr == NULL) return false;
    if (((validChar(attr->name) == 0) && (strcmp(attr->name, "") != 0)) || ((validChar(attr->value) == 0) && (strcmp(attr->value, "") != 0))) return false; // may be empty, may not be null
    return true;
}

// validates the attribute structs against specifications
bool validAttrListStruct(List* otherAttributes){

    if (otherAttributes == NULL) return false;

    void* elem;

    ListIterator iter = createIterator(otherAttributes); // traverse through the attribute
    while ((elem = nextElement(&iter)) != NULL){
        Attribute* attr = (Attribute*) elem;
        // check for initialized name and value
        if (validAttrStruct(attr) == false) return false;
    }

    return true;

}

// validates single rectangle structure
bool validRectStruct(Rectangle* rect){

    if (rect == NULL) return false;

    // check for initialized units
    if ((validChar(rect->units) == 0) && (strcmp(rect->units, "") != 0)) return false; // may be empty, may not be null

    // check for valid range: >= 0
    if (checkRange(rect->width) == false) return false;
    if (checkRange(rect->height) == false) return false;

    // initialized list?
    if (rect->otherAttributes == NULL) return false;

    // if not empty, check for valid attributes structs
    if ((isListEmpty(rect->otherAttributes) == 0) && (validAttrListStruct(rect->otherAttributes) == false)) return false;
    return true;

}

// validates the rect structs against specifications
bool validRectListStruct(List* rectangles){

    if (rectangles == NULL) return false;

    void* elem;

    ListIterator iter = createIterator(rectangles); // traverse through the rectangle list
    while ((elem = nextElement(&iter)) != NULL){
        Rectangle* rect = (Rectangle*) elem;
        if (validRectStruct(rect) == false) return false;
    }

    return true;
}

// validates single circle struct
bool validCircStruct(Circle* circ){

    if (circ == NULL) return false;

    // check for initialized units
    if ((validChar(circ->units) == 0) && (strcmp(circ->units, "") != 0)) return false; // may be empty, may not be null

    // check for valid range: >= 0
    if (checkRange(circ->r) == false) return false;

    // initialized list?
    if (circ->otherAttributes == NULL) return false;

    // check for valid attributes structs
    if ((isListEmpty(circ->otherAttributes) == 0) && (validAttrListStruct(circ->otherAttributes) == false)) return false;

    return true;
}

// validates the circle structs against specifications
bool validCircListStruct(List* circles){

    if (circles == NULL) return false;

    void* elem;

    ListIterator iter = createIterator(circles); // traverse through the rectangle list
    while ((elem = nextElement(&iter)) != NULL){
        Circle* circ = (Circle*) elem;
        if (validCircStruct(circ) == false) return false;
    }

    return true;
}

// validates path structure
bool validPathStruct(Path* path){

    if (path == NULL) return false;

    // check for initialized units
    if ((validChar(path->data) == 0) && (strcmp(path->data, "") != 0)) return false; // may be empty, may not be null

    // initialized list?
    if (path->otherAttributes == NULL) return false;

    // check for valid attributes structs
    if ((isListEmpty(path->otherAttributes) == 0) && (validAttrListStruct(path->otherAttributes) == false)) return false;

    return true;
}

// validates the path structs against specifications
bool validPathListStruct(List* paths){

    if (paths == NULL) return false;

    void* elem;

    ListIterator iter = createIterator(paths); // traverse through the rectangle list
    while ((elem = nextElement(&iter)) != NULL){
        Path* path = (Path*) elem;
        if (validPathStruct(path) == false) return false;
    }

    return true;
}

// validates a single group struct
bool validGroupStruct(Group* group){

    if (group == NULL) return false;

    // 1. initialized values, is the list NULL?
    if ((group->rectangles == NULL) ||
       (group->circles == NULL) ||
       (group->paths == NULL) ||
       (group->groups == NULL) ||
       (group->otherAttributes == NULL)) return false;

    // 2. list contents valid?
    // if the list is not empty, check if the content is valid
    if ((isListEmpty(group->otherAttributes) == 0) && (validAttrListStruct(group->otherAttributes) == false)) return false;
    if ((isListEmpty(group->rectangles) == 0) && (validRectListStruct(group->rectangles) == false)) return false;
    if ((isListEmpty(group->circles) == 0) && (validCircListStruct(group->circles) == false)) return false;
    if ((isListEmpty(group->paths) == 0) && (validPathListStruct(group->paths) == false)) return false;
    if ((isListEmpty(group->groups) == 0) && (validGroupListStruct(group->groups) == false)) return false;

    return true;
}

// validates the group structs against specifications, as well as the inner lists structs
bool validGroupListStruct(List* groupList){

    if (groupList == NULL) return false;

    void* elem;

    ListIterator iter = createIterator(groupList); // traverse through the rectangle list
    while ((elem = nextElement(&iter)) != NULL){
        Group* group = (Group*) elem;
        if (validGroupStruct(group) == false) return false;
    }

    // 4. success
    return true;

}

// for validating the circle and rectangle width height and radius
bool checkRange(float number){

    if (number < 0) return false;
    //do we have to check for range in a floating point number? isfinate?
    return true;

}

// for validating list contents
bool isListEmpty(List* list){

    if (getLength(list) == 0) return 1; // 1 means true, it is empty
    return 0; // 0 indicate that it is NOT empty
}

/*
    the following funcitons with signiture changeValueInShape will traverse the shape list for the specified shape, and
    if attribute name is specified as a definition, set it
    if it is not specified as a deifnition, call changeValueInattribute to try to find it in other attibrutes
    if it is not in the other attrbutes, append it to the other attributes list
    strcpy when updating the strings will not reult in memory leaks
    if any error occurs (ie, null values, invalid values, conversion error, etc) they will return false
*/
// loops through attributes to change the value. if not present, appends to the end of the list
bool changeValueInAttr (List* attrList, Attribute* newAttribute){

    if (attrList == NULL || newAttribute == NULL) return false;

    bool found = false;
    void* elem;
    ListIterator iter = createIterator(attrList); // traverse through the other attributes
    while ((elem = nextElement(&iter)) != NULL){
        Attribute* attr = (Attribute*) elem;
        if (strcasecmp(attr->name, newAttribute->name) == 0){
            found = true;
            if (validAttrStruct(newAttribute) == false) return false;
            strcpy(attr->value, newAttribute->value);
            deleteAttribute((void*) newAttribute);
            break;
        }
    }

    if (found == false){ // append to list if not found
        insertBack(attrList, (void*)newAttribute);
    }
    return true;
}

// loops through rectangles to change the value. if not present, checks other attributes list
bool changeValueInRect (List* rectList, int index, Attribute* newAttribute){

    if (rectList == NULL || newAttribute == NULL) return false;

    int i = 0;
    bool found = false;
    void* elem;
    ListIterator iter = createIterator(rectList); // traverse through the rects
    while ((elem = nextElement(&iter)) != NULL){
        Rectangle* rect = (Rectangle*) elem;

        // reached the index of the rect struct that we want to adjust
        if (i == index){
            if (strcasecmp(newAttribute->name, "x") == 0){
                found = numberWithUnits(&(rect->x), rect->units, newAttribute->value); // since units dont need ot be updated, the funciton will not update it
                deleteAttribute((void*) newAttribute);
            }
            else if (strcasecmp(newAttribute->name, "y") == 0){
                found = numberWithUnits(&(rect->y), rect->units, newAttribute->value);
                deleteAttribute((void*) newAttribute);
            }
            else if (strcasecmp(newAttribute->name, "width") == 0){
                found = numberWithUnits(&(rect->width), rect->units, newAttribute->value);
                // must also check if it is valid number?
                // if (rect->width < 0) return false;
                deleteAttribute((void*) newAttribute);
            }
            else if (strcasecmp(newAttribute->name, "height") == 0){
                found = numberWithUnits(&(rect->height), rect->units, newAttribute->value);
                // if (rect->height < 0) return false;
                deleteAttribute((void*) newAttribute);
            }
            else{
                found = changeValueInAttr(rect->otherAttributes, newAttribute);
            }
            break;
        }
        ++i;
    }

    return found;
}

// loops through circles to change the value. if not present, checks other attributes list
bool changeValueInCirc (List* circList, int index, Attribute* newAttribute){

    if (circList == NULL || newAttribute == NULL) return false;

    int i = 0;
    bool found = false;
    void* elem;
    ListIterator iter = createIterator(circList); // traverse through the circle
    while ((elem = nextElement(&iter)) != NULL){
        Circle* circ = (Circle*) elem;

        // reached the index of the circle struct that we want to adjust
        if (i == index){
            if (strcasecmp(newAttribute->name, "cx") == 0){
                found = numberWithUnits(&(circ->cx), circ->units, newAttribute->value); // since units dont need ot be updated, the funciton will not update it
                deleteAttribute((void*) newAttribute);
            }
            else if (strcasecmp(newAttribute->name, "cy") == 0){
                found = numberWithUnits(&(circ->cy), circ->units, newAttribute->value);
                deleteAttribute((void*) newAttribute);
            }
            else if (strcasecmp(newAttribute->name, "r") == 0){
                found = numberWithUnits(&(circ->r), circ->units, newAttribute->value);
                // if (circ->r < 0) return false;
                deleteAttribute((void*) newAttribute);
            }
            else{
                found = changeValueInAttr(circ->otherAttributes, newAttribute);
            }
            break;
        }
        ++i;
    }

    return found;
}

// loops through paths to change the value. if not present, checks other attributes list
bool changeValueInPath (List* pathList, int index, Attribute* newAttribute){

    if (pathList == NULL || newAttribute == NULL) return false;

    int i = 0;
    bool found = false;
    void* elem;
    ListIterator iter = createIterator(pathList); // traverse through the path
    while ((elem = nextElement(&iter)) != NULL){
        Path* path = (Path*) elem;

        // reached the index of the path struct that we want to adjust
        if (i == index){
            if (strcasecmp(newAttribute->name, "d") == 0){
                found = true;
                strcpy(path->data, newAttribute->value);
                deleteAttribute((void*) newAttribute);
            }
            else{
                found = changeValueInAttr(path->otherAttributes, newAttribute);
            }
            break;
        }
        ++i;
    }

    return found;
}

// loops through groups other attributes list to change the value. if not present, appends to list
bool changeValueInGroup (List* groupList, int index, Attribute* newAttribute){

    if (groupList == NULL || newAttribute == NULL) return false;

    int i = 0;
    bool found = false;
    void* elem;
    ListIterator iter = createIterator(groupList); // traverse through the path
    while ((elem = nextElement(&iter)) != NULL){
        Group* group = (Group*) elem;

        // reached the index of the path struct that we want to adjust
        if (i == index){
            // in a group, we do not modify items in the inner lists
            found = changeValueInAttr(group->otherAttributes, newAttribute);
            break;
        }
        ++i;
    }

    return found;
}
