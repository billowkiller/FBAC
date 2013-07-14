/*
 * =====================================================================================
 *
 *       Filename:  sniff.c
 *
 *    Description:  sniff traffic
 *    				* sniff tcp message
 *    				* parse http message
 *    				* filter http content
 *
 *        Version:  1.0
 *        Created:  01/25/2013 03:50:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Billowkiller (bk), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <glib.h>
#include "trafficMonitor.h"

extern int pipe_config();
extern GHashTable* hash_config;

int main()
{
	pipe_config();

	monitor();
}
