#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "SVGHelper2.h"
#include "SVGParser.h"

#ifdef LIBXML_TREE_ENABLED

static void get_element_names(xmlNode * a_node, SVG * svg){

    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {

        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
        }

        if (cur_node->content != NULL ){
             printf("  content: %s\n", cur_node->content);
        }

        if (strcmp(cur_node->name, "title") == 0){
            verifyCopy(svg->title, cur_node->content, sizeof(svg->title), sizeof(cur_node->content));
            // title attributes are in the otherAttributes list
        }
        else if (strcmp(cur_node->name, "desc") == 0){
            verifyCopy(svg->description, cur_node->content, sizeof(svg->description), sizeof(cur_node->content));
            // desc attributes are in the otherAttributes list
        }

        // Iterate through every attribute of the current node
        xmlAttr *attr;
        for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);

            Attribute * anAtr = malloc(sizeof(Attribute) + sizeof(cont));
            anAtr->name = malloc(sizeof(attrName));
            strcpy(anAtr->name, attrName);
            strcpy(anAtr->value, cont);

//            insertBack(list, (void*)anAtr);

            printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);
        }

        get_element_names(cur_node->children, svg);
    }

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
