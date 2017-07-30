#include "olsr.h"

/*PUBLIC FUNCTIONS*/
/**
 * Initalize a new olsr plugin handler
 * @param host host address as a string
 * @param proto type of the remote plugin (0->netjson 1->jsoninfo)
 * @return pointer to olsr plugin handler
 */
routing_plugin* new_plugin(char* host, int port, c_graph_parser *gp, int json_type)
{
	routing_plugin *o = (routing_plugin *) malloc(sizeof(routing_plugin));
	o->port=port;
	o->host=strdup(host);
	o->gp = gp;
	o->recv_buffer=0;
	o->self_id=0;
	o->json_type=json_type;
	return o;
}


/**
 * Get the topology data from host
 * @param olsr plugin handler object
 * @return 1 if success, 0 otherwise
 */
int get_topology(routing_plugin *o) /*netjson & jsoninfo*/
{
	int sent;
	if((o->sd= _create_socket(o->host, o->port))==0){
		printf("Cannot connect to %s:%d", o->host, o->port);
		return 0;
	}
	switch(o->json_type){
	case 1:{
		/*netjson*/
		char *req = "/NetworkGraph";
		if( (sent = send(o->sd,req,strlen(req),MSG_NOSIGNAL))==-1){
			printf("Cannot send to %s:%d", o->host, o->port);
			close(o->sd);
			return 0;
		}
		if(o->recv_buffer!=0){
			free(o->recv_buffer);
			o->recv_buffer=0;
		}
		if(!_telnet_receive(o->sd, &(o->recv_buffer))){
			printf("cannot receive \n");
			close(o->sd);
			return 0;
		}
		o->t = parse_netjson(o->recv_buffer);
		if(!o->t){
			printf("can't parse netjson\n %s \n", o->recv_buffer);
			close(o->sd);
			return 0;
		}
		}
		break;

	case 0:{
		/*jsoninfo*/
		char *req = "/topology";
		sent = write(o->sd,req,strlen(req));
		if(!_receive_data(o->sd, &(o->recv_buffer))){
			close(o->sd);
			return 0;
		}
		struct topology *t = parse_jsoninfo(o->recv_buffer);
		graph_parser_parse_simplegraph(o->gp, t);
		destroy_topo(t);
		}
		break;

	default:
		close(o->sd);
		return 0;
	}
	close(o->sd);
	return 1;
}




/**
 * Push the timers value to the routing daemon
 * @param olsr plugin handler object
 * @return 1 if success, 0 otherwise
 */
int push_timers(routing_plugin *o, struct timers t)
{
	/*TODO: push h and tc value to the daemon*/
	printf("%f \t %f\n", t.h_timer, t.tc_timer);
	o->sd =_create_socket(o->host, o->port);
	char cmd[111];
	sprintf(cmd, "/HelloTimer=%4.2f/TcTimer=%4.2f", t.h_timer, t.tc_timer);
	write(o->sd, cmd, strlen(cmd));
	printf("Pushed Timers %4.2f  %4.2f\n", t.tc_timer, t.h_timer);
	close(o->sd);
	return 1;
}

/**
 * Delete the olsr plugin handler
 * @param olsr plugin handler object
 */
void delete_plugin(routing_plugin* o)
{
	free(o->host);
	if(o->recv_buffer!=0)
  	free(o->recv_buffer);
	free(o->self_id);
	free(o);
}