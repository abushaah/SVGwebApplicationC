#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "SVGParser.h"

#define DELIMITERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ -=:;"
#define NUMDELIMITERS "0123456789."

#ifdef LIBXML_TREE_ENABLED

static void get_element_names(xmlNode * a_node, SVG * svg);

Rectangle* rectAttributes(xmlNode *cur_node);
Attribute* otherAttributes (char *name, char *content);

int numberWithUnits(float* number, char* units, char* value);
void verifyCopy(char* field, xmlChar * data, size_t fLength, size_t dLength);
int validChar(char* word);

static void get_element_names(xmlNode * a_node, SVG * svg){

    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {

        if (strcasecmp(cur_node->name, "title") == 0){
            verifyCopy(svg->title, cur_node->content, sizeof(svg->title), sizeof(cur_node->content));
            // title attributes are in the otherAttributes list
        }
        else if (strcasecmp(cur_node->name, "desc") == 0){
            verifyCopy(svg->description, cur_node->content, sizeof(svg->description), sizeof(cur_node->content));
            // desc attributes are in the otherAttributes list
        }
        else if (strcasecmp(cur_node->name, "rect") == 0){ // create new rectangle
            Rectangle* rect = rectAttributes(cur_node); // fill in with attributes
            insertBack(svg->rectangles, (void*)rect); // insert into the rectangle list
        }
        // repeat the same for circle, path, groups
        else{ // just call attr with no argument and place in otherAttributes list
        }

        get_element_names(cur_node->children, svg);
    }

    // keep count of the number of elements created, since if it is 0 must create a new empty struct to avoid it being null

}

/**
 * This function will return an attribute struct when given a node and its attributes
 * caller must free the node
 */

Attribute* otherAttributes (char *name, char *content){

    Attribute* anAtr = malloc(sizeof(Attribute) + strlen(content) + 1); // 1 for null
    anAtr->name = malloc(strlen(name) + 1); // 1 for null
    strcpy(anAtr->name, name);
    strcpy(anAtr->value, content);

    return anAtr;

}

/**
 * This function will return a rectangle struct with its attributes when given a node
 * caller must free the node
 */

Rectangle* rectAttributes(xmlNode *cur_node){ // fills in attributes for a rectangle

    // Iterate through every attribute of the current rectangle node
    xmlAttr *attr;
    Rectangle* rect = malloc(sizeof(Rectangle));

    rect->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes); // must initialize list, cannot be NULL but can be empty

    int valid[4] = {0, 0, 0, 0}; // x, y, w, h, if one of these is 1 in the end, then it is not valid and must be set to default value

    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);

        if (strcasecmp(attrName, "x") == 0){
            valid[0] = numberWithUnits(&(rect->x), rect->units, cont);
        }
        else if (strcasecmp(attrName, "y") == 0){
            valid[1] = numberWithUnits(&(rect->y), rect->units, cont);
        }
        else if (strcasecmp(attrName, "width") == 0){
            valid[2] = numberWithUnits(&(rect->width), rect->units, cont);
        }
        else if (strcasecmp(attrName, "height") == 0){
            valid[3] = numberWithUnits(&(rect->height), rect->units, cont);
        }
        else{ // place in otherAttributes list
          insertBack(rect->otherAttributes, (void*)otherAttributes (attrName, cont)); // create an attribute node and place it in the list
        }
    }

    // if units are emtpy, emtpy string, etc assumes that all units are the same if specified
    if (valid[0] == 0) rect->x = 0; // x is not specified
    if (valid[1] == 0) rect->y = 0; // y is not specified
    if (valid[2] == 0) rect->width = 0; // w is not specified
    if (valid[3] == 0) rect->height = 0; // h is not specified
    if (validChar(rect->units) == 0) strcpy(rect->units, ""); // u is not specified

    return rect;
}

/**
 * verifyCopy verifies the data fits into the fields
 * if it exceeds, truncuates the data to fit
 */
void verifyCopy(char* field, xmlChar * data, size_t fLength, size_t dLength){

    if (data == NULL){
        strcpy(field, "");
        return;
    }

    if (fLength < dLength){ // data is greater than allocated
        data[fLength - 1] = '\0';
    }

    strcpy(field, data);
}

int numberWithUnits(float* number, char* units, char* value){

    char* cpy = malloc (sizeof(value));
    strcpy(cpy, value);

    if (value == NULL) return 0;

    char* token = strtok(value, DELIMITERS);

    *number = atof (token);

    token = strtok(cpy, NUMDELIMITERS);
    if (token != NULL) strcpy(units, token);

    free(cpy);
    return 1;
}

int validChar(char* word){ // checks if a character is valid

    if (word == NULL || word[0] == '\0' || word[0] == '\n' || strcmp(word, "") == 0) return 0; // 0 indicates false, not valid
    return 1;
}

/*
int nameSpace(char *field, const xmlChar * data, size_t fLength, size_t dLength){

    char *dataCpy = malloc(sizeof(data)); // must create a copy since it is a constant and cannot be changed
    strcpy(data, dataCpy);

    if (dataCpy == NULL || dataCpy[0] == '\0' || dataCpy[0] == '\n' || strcmp(dataCpy, "") == 0) return 0;

    if (fLength < dLength){ // data is greater than allocated
        dataCpy[fLength - 1] = '\0';
    }

    strcpy(field, data);
    free(dataCpy);

}
*/

#else
int main(void) {
    fprintf(stderr, "Tree support not compiled in\n");
    return 0;
}
#endif

