// -----------------------------------------------------------------------------
// main - MPF 01/2020
// -----------------------------------------------------------------------------

// includes
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "regex_lib.h"

// defines
#define STR_SIZE    4096

// file pointers
FILE * pfsource = NULL;
FILE * pfgraph = NULL;

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
int main(int argc, char *argv[]){

    char str[STR_SIZE];
    char tmpstr[STR_SIZE];
    char state[STR_SIZE];
    unsigned int start[REGEX_MAX_GROUPS];
    unsigned int len[REGEX_MAX_GROUPS];
    unsigned int nsub;
    regex_err_t err;

    #define CLOSE_FILES \
        if(pfsource!=NULL) fclose(pfsource);\
        if(pfgraph!=NULL) fclose(pfgraph);\

    // 3 arguments: exe <source> <destination>
    if (argc!=3) {
        printf("Usage: smgraph <source.c> <output.gv>:\n");
        printf("source.c : input file that has the fsm code\n");
        printf("output.gv: output file that graphviz code is written to\n");
        exit(1);
    }

    // open source file for read access
    pfsource = fopen(argv[1],"r");
    if (pfsource == NULL){
        printf("Cannot open source file '%s'\n", argv[1]);
        CLOSE_FILES;
        exit(1);
    } 

    // open graph for write access
    pfgraph = fopen(argv[2],"w");
    if (pfgraph == NULL){
        printf("Cannot open output graph file '%s'\n", argv[2]);
        CLOSE_FILES;
        exit(1);
    }

    // regex strings
    const char fsm_init_str[] = "^\\s*FSM_INIT\\s*\\(\\s*(\\S+)\\s*\\)";
    const char begin_state_str[] = "^\\s*BEGIN_STATE\\s*\\(\\s*(\\S+)\\s*\\)";
    const char end_state_str[] = "^\\s*END_STATE";
    const char begin_entry_str[] = "^\\s*BEGIN_ENTRY";
    const char end_entry_str[] = "^\\s*END_ENTRY";
    const char begin_during_str[] = "^\\s*BEGIN_DURING";
    const char end_during_str[] = "^\\s*END_DURING";
    const char begin_exit_str[] = "^\\s*BEGIN_EXIT";
    const char end_exit_str[] = "^\\s*END_EXIT";
    const char begin_event_str[] = "^\\s*BEGIN_EVENT\\s*\\(\\s*(\\S+)[^,]*,\\s*(\\S+)\\s*\\)";
    const char end_event_str[] = "^\\s*END_EVENT";
    const char comment_str[] = "^\\s*\\/\\/\\s*(.+)[^\\n]";

    // print header
    fprintf(pfgraph, "digraph G {\n");
    fprintf(pfgraph, "    node [shape=box,fontname=courier];\n");

    // find init
    do {
        if(fgets(str, STR_SIZE, pfsource)!=NULL) {
            // find a FSM INIT
            err = regex_search(str, fsm_init_str, start, len, &nsub);
            if (err==REGEX_ERR_NONE) {
                // init found
                regex_extract(tmpstr, str, start[1], len[1]);                
                fprintf(pfgraph, "    ENTRY [label=\"ENTRY\"];\n");
                fprintf(pfgraph, "    ENTRY -> %s;\n", tmpstr);
                break;
            }
        }
    } while (!feof(pfsource));

    // parse states
    do {
        if(fgets(str, STR_SIZE, pfsource)!=NULL) {
            // find a BEGIN_STATE
            err = regex_search(str, begin_state_str, start, len, &nsub);
            if (err==REGEX_ERR_NONE) {
                // state found
                regex_extract(tmpstr, str, start[1], len[1]);
                fprintf(pfgraph, "    %s [label=\"=== %s ===\\n", tmpstr, tmpstr);
                do {                    
                    if(fgets(str, STR_SIZE, pfsource)!=NULL) {

                        // find BEGIN_ENTRY
                        err = regex_search(str, begin_entry_str, start, len, &nsub);
                        if (err==REGEX_ERR_NONE) {
                            fprintf(pfgraph, "Entry:\\l");
                            do {
                                if(fgets(str, STR_SIZE, pfsource)!=NULL){
                                    // find comment
                                    err = regex_search(str, comment_str, start, len, &nsub);
                                    if (err==REGEX_ERR_NONE) {
                                        regex_extract(tmpstr, str, start[1], len[1]);
                                        fprintf(pfgraph, "  %s\\l", tmpstr);
                                        continue;
                                    }
                                    // find END_ENTRY
                                    err = regex_search(str, end_entry_str, start, len, &nsub);
                                    if (err==REGEX_ERR_NONE) {
                                        // exit BEGIN ENTRY
                                        break;
                                    }
                                }
                            } while (!feof(pfsource));
                        }

                        // find BEGIN_DURING
                        err = regex_search(str, begin_during_str, start, len, &nsub);
                        if (err==REGEX_ERR_NONE) {
                            fprintf(pfgraph, "During:\\l");
                            do {
                                if(fgets(str, STR_SIZE, pfsource)!=NULL){
                                    // find comment
                                    err = regex_search(str, comment_str, start, len, &nsub);
                                    if (err==REGEX_ERR_NONE) {
                                        regex_extract(tmpstr, str, start[1], len[1]);
                                        fprintf(pfgraph, "  %s\\l", tmpstr);
                                        continue;
                                    }
                                    // find END_DURING
                                    err = regex_search(str, end_during_str, start, len, &nsub);
                                    if (err==REGEX_ERR_NONE) {
                                        // exit BEGIN DURING
                                        break;
                                    }
                                }
                            } while (!feof(pfsource));
                        }

                        // find BEGIN_EXIT
                        err = regex_search(str, begin_exit_str, start, len, &nsub);
                        if (err==REGEX_ERR_NONE) {
                            fprintf(pfgraph, "Exit:\\l");
                            do {
                                if(fgets(str, STR_SIZE, pfsource)!=NULL){
                                    // find comment
                                    err = regex_search(str, comment_str, start, len, &nsub);
                                    if (err==REGEX_ERR_NONE) {
                                        regex_extract(tmpstr, str, start[1], len[1]);
                                        fprintf(pfgraph, "  %s\\l", tmpstr);
                                        continue;
                                    }
                                    // find END_EXIT
                                    err = regex_search(str, end_exit_str, start, len, &nsub);
                                    if (err==REGEX_ERR_NONE) {
                                        // exit BEGIN EXIT
                                        break;
                                    }
                                }
                            } while (!feof(pfsource));
                        }

                        // find END_STATE
                        err = regex_search(str, end_state_str, start, len, &nsub);
                        if (err==REGEX_ERR_NONE) {
                            fprintf(pfgraph, "\"];\n");
                            break;
                        }

                    }
                } while (!feof(pfsource));

            }
        }

    } while (!feof(pfsource));

    // parse events
    rewind(pfsource);
    int begin_state_found=0;
    int all_state_created=0;
    do {
        if(fgets(str, STR_SIZE, pfsource)!=NULL) {
            // find a BEGIN_EVENT before BEGIN_FSM
            if (!begin_state_found){
                err = regex_search(str, begin_event_str, start, len, &nsub);
                if (err==REGEX_ERR_NONE) {
                    if (!all_state_created){
                        fprintf(pfgraph, "    ALL [label=\"ALL\"];\n");
                        all_state_created = 1;
                    }
                    fprintf(pfgraph, "    ALL -> ");
                    regex_extract(tmpstr, str, start[2], len[2]);
                    fprintf(pfgraph, "%s ", tmpstr);
                    regex_extract(tmpstr, str, start[1], len[1]);
                    fprintf(pfgraph, "[fontname=courier,label=\"%s", tmpstr);
                    do {
                        if(fgets(str, STR_SIZE, pfsource)!=NULL){
                            // // find comment
                            // err = regex_search(str, comment_str, start, len, &nsub);
                            // if (err==REGEX_ERR_NONE) {
                            //     regex_extract(tmpstr, str, start[1], len[1]);
                            //     fprintf(pfgraph, "  %s\\l", tmpstr);
                            //     continue;
                            // }
                            // find END_EVENT
                            err = regex_search(str, end_event_str, start, len, &nsub);
                            if (err==REGEX_ERR_NONE) {
                                // exit BEGIN EVENT
                                fprintf(pfgraph, "\"];\n");
                                break;
                            }
                        }
                    } while (!feof(pfsource));
                }
            }
            // find a BEGIN_STATE
            err = regex_search(str, begin_state_str, start, len, &nsub);
            if (err==REGEX_ERR_NONE) {
                // state found
                begin_state_found = 1;
                regex_extract(state, str, start[1], len[1]);
                do {                    
                    if(fgets(str, STR_SIZE, pfsource)!=NULL) {

                        // find BEGIN_EVENT
                        err = regex_search(str, begin_event_str, start, len, &nsub);
                        if (err==REGEX_ERR_NONE) {
                            fprintf(pfgraph, "    %s -> ", state);
                            regex_extract(tmpstr, str, start[2], len[2]);
                            fprintf(pfgraph, "%s ", tmpstr);
                            regex_extract(tmpstr, str, start[1], len[1]);
                            fprintf(pfgraph, "[fontname=courier,label=\"%s", tmpstr);
                            do {
                                if(fgets(str, STR_SIZE, pfsource)!=NULL){
                                    // // find comment
                                    // err = regex_search(str, comment_str, start, len, &nsub);
                                    // if (err==REGEX_ERR_NONE) {
                                    //     regex_extract(tmpstr, str, start[1], len[1]);
                                    //     fprintf(pfgraph, "  %s\\l", tmpstr);
                                    //     continue;
                                    // }
                                    // find END_EVENT
                                    err = regex_search(str, end_event_str, start, len, &nsub);
                                    if (err==REGEX_ERR_NONE) {
                                        // exit BEGIN EVENT
                                        fprintf(pfgraph, "\"];\n");
                                        break;
                                    }
                                }
                            } while (!feof(pfsource));
                        }

                        // find END_STATE
                        err = regex_search(str, end_state_str, start, len, &nsub);
                        if (err==REGEX_ERR_NONE) {
                            break;
                        }
                    }
                } while (!feof(pfsource));
            }
        }
    } while (!feof(pfsource));

    // close graph
    fprintf(pfgraph, "}\n");

    // close all files
    CLOSE_FILES;

    return 0;
}

