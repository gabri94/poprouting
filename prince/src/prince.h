/*
Copyright (c) 2016 Gabriele Gemmi <gabriel@autistici.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
/*
 * prince.h
 *
 *  Created on: 12 mag 2016
 *      Author: gabriel
 */

#ifndef SRC_PRINCE_H_
#define SRC_PRINCE_H_

#include <math.h>
#include <dlfcn.h>
#include "common.h"
#include "lib/ini.h"
#include "parser.h"

struct constants{
	double O_H, O_TC, sq_lambda_H, sq_lambda_TC, R;
};


struct prince_handler{
	struct timers def_t, opt_t;
	struct constants c;
	char *self_id, *host;
	c_graph_parser *gp;
	map_id_degree_bc *bc_degree_map;
	routing_plugin *rp;
	int proto,port,refresh,json_type;
	bool heuristic, weights, recursive, stop_unchanged, multithreaded;
	void *plugin_handle;
};


int main(int argc, char *argv[]);

struct prince_handler* new_prince_handler(char * conf_file);
int compute_constants(struct prince_handler *ph);
int compute_timers(struct prince_handler *ph);
void delete_prince_handler(struct prince_handler*);
int read_config_file(struct prince_handler *ph, char *filepath);

static int handler(void* user, const char* section, const char* name, const char* value)
{
    struct prince_handler* pconfig = ( struct prince_handler*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    /*section :proto*/
    if (MATCH("proto", "protocol")) {
    	if(strcmp(value, "olsr")==0)	pconfig->proto = 0;
    	else if(strcmp(value, "oonf")==0)	pconfig->proto = 1;
    } else if (MATCH("proto", "host")) {
        pconfig->host = strdup(value);
    } else if (MATCH("proto", "port")) {
        pconfig->port = atoi(value);
    }else if (MATCH("proto", "self_id")) {
        pconfig->self_id = strdup(value);
    }else if (MATCH("proto", "refresh")) {
        pconfig->refresh = atoi(value);
    }
    /*section :graph-parser*/
    else if (MATCH("graph-parser", "heuristic")) {
        pconfig->heuristic = atoi(value);
    } else if (MATCH("graph-parser", "weights")) {
        pconfig->weights = atoi(value);
    }
    /* unknown section/name, error */
    else {
        return 0;
    }
    return 1;
}

#endif /* SRC_PRINCE_H_ */
