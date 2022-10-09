#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <sstream>

using namespace std;

#define PORTb 31430
#define UDP_PORT 33430 
#define localhost "127.0.0.1"

const int INF = (int)1000000;               //infinite

set<int> vertex;                            //store vertexes in map.txt
map<int, vector<pair<int, float> > > graph; //for Dijkstra
void readmap() {
	ifstream mapread("map.txt", ios::in);
	string l;
	while (getline(mapread, l)) {
		vector<string> verdis;
		const char *delim = " ";
		char *strs = new char[l.length() + 1];
		strcpy(strs, l.c_str());
		char *p = strtok(strs, delim);
		while (p) {
			string s = p;
			verdis.push_back(s);
			p = strtok(NULL, delim);
		}
		int vertice1 = atoi(verdis[0].c_str());
		int vertice2 = atoi(verdis[1].c_str());
		float dist = atof(verdis[2].c_str());

		// build adjacency map
		pair<int,float> pair1(vertice2, dist);  // pair second node and distance
		if (graph.count(vertice1) > 0) {
			vector<pair<int, float> > cur_vec = graph[vertice1];
			cur_vec.push_back(pair1);
			graph[vertice1] = cur_vec;
		} else {
			vector<pair<int, float> > cur_vec;
			cur_vec.push_back(pair1);
			graph[vertice1] = cur_vec;
		}
		pair<int, float> pair2(vertice1, dist); // pair first node and distance
		if (graph.count(vertice2) > 0) {
			vector<pair<int, float> > cur_vec = graph[vertice2];
			cur_vec.push_back(pair2);
			graph[vertice2] = cur_vec;
		} else {
			vector<pair<int, float> > cur_vec;
			cur_vec.push_back(pair2);
			graph[vertice2] = cur_vec;
		}
		vertex.insert(vertice1);
		vertex.insert(vertice2);
	}
}

// find the shortest path from given source vertex to any other vertices 
int path_finding(int src, map<int, float> &result,int s) {
	// store the vertices involved
	set<int> vertices = vertex;

	// cur_graph stores every vertex and its adjacent vertices along with the distance between them
	map<int, vector<pair<int, float> > > cur_graph = graph;

	set<int>::iterator iter = vertices.begin();
	// vis map stores whether a vertex has been visited
	map<int, bool> vis;

	//find src and location
	int f = 0;
	int j = 0;

	// init every node distance as INF
	while(iter != vertices.end()) {
		int cur_ver = *iter;
		if (cur_ver == src){
			f++;
		}else {
			result[cur_ver] = INF;
		}
		if(cur_ver == s){j++;}
		vis[cur_ver] = false;
		iter++;
	}
	if(f==0){
		printf("Initialization error: wrong location of Hospital\n");
		return 0;
	}
	if(j==0){
		printf("Hospital A does not have the location %d in map\n",s);
		return 0;
	}

	map<int, bool>::iterator iter_vis = vis.begin();
	while(iter_vis != vis.end()) {
		int u = -1;
		int MIN = INF;
		map<int, bool>::iterator iter_vis1 = vis.begin();
		while (iter_vis1 != vis.end()) {
			int cur_ver = iter_vis1 -> first;
			bool state = iter_vis1 -> second;
			if (state == false && result[cur_ver] < MIN) {
				u = cur_ver;
				MIN = result[cur_ver];
			}
			iter_vis1++;
		}
		vis[u] = true;

		vector<pair<int, float> > neighbors = cur_graph[u];
		for (uint i = 0; i < neighbors.size(); i++) {
			int v = neighbors[i].first;
			float dist = neighbors[i].second;
			// if the vertex hasn't been visted and find a shorter path than the previous one
			if (vis[v] == false && result[u] + dist < result[v]) {
				result[v] = result[u] + dist;
			}
		}
		iter_vis++;
	}
	return 1;
}

// return distance of the shortest path 
float path_dis(map<int, float>&bmap, int location, int s) {
	// iterate through the result
	float d;
	map<int, float>::iterator iter = bmap.begin();
	while (iter != bmap.end()) {
		int des = iter->first;      // location needs to be found
		float len = iter->second;   // the shortest distance
		if (des == location) {
			iter++;
			continue;
		}
		if(s == des){d = len;}
		if(s == location){d = 0;}
		iter++;
	}
	return d;
}
int main(int argc,char *argv[])
{	
	int a = atoi(argv[1]);
	int cap_total= atoi(argv[2]);
	int occupation=atoi(argv[3]);
	float dis;
	printf("Hospital B is up and running using UDP on port %d\n",PORTb);
	printf("Hospital B has total capacity %d and initial occupancy %d\n",cap_total,occupation);
	readmap();
	//create an udp socket
	//reference: Beej's
	int udp_sockfd = socket(AF_INET, SOCK_DGRAM,0);//IPv4
	if(udp_sockfd < 0){
		perror("socket error\n");
		exit(1);
	}

	struct sockaddr_in addr_serv;
	int len;
	memset(&addr_serv, 0 ,sizeof (struct sockaddr_in));//fill every byte with 0
	addr_serv.sin_family = AF_INET;                    //IPv4
	addr_serv.sin_port = htons(PORTb);                 //port #
	addr_serv.sin_addr.s_addr = inet_addr(localhost);  //IP add
	len = sizeof(addr_serv);

	//bind socket
    //reference: Beej's
	if(bind(udp_sockfd,(struct sockaddr *)&addr_serv,sizeof(addr_serv))<0)
	{
		perror("bind error\n");
		exit(1);
	}

	int recv_num;
	int send_num;
	char send_buf[20];
	char recv_buf[20];
	char capa[3];
	struct sockaddr_in addr_client;
	int len2;
	memset(&addr_client, 0 ,sizeof (struct sockaddr_in));//fill every byte with 0
	addr_client.sin_family = AF_INET;//IPv4
	addr_client.sin_port = htons(UDP_PORT);//port #
	addr_client.sin_addr.s_addr = inet_addr(localhost);//IP add
	len2 = sizeof(addr_client);

	while(1){
		//send capacity and occupation
        //reference: Beej's
		stringstream capt;
        capt << cap_total << " "<< occupation; 
		string cst = capt.str();
		strcpy(send_buf,cst.c_str());
		send_num = sendto(udp_sockfd, send_buf, 100,0,(struct sockaddr *)&addr_client, len2);
		if(send_num<0){
			perror("sendto error\n");
			exit(1);
		}
		
		float ava;
		ava = (float) (cap_total - occupation)/cap_total;
		if(ava>1||ava<0){
			printf("Hospital B has None availability\n");	
		}

		//get location
	    //refernce: Beej's
		recv_num = recvfrom(udp_sockfd,recv_buf,sizeof(recv_buf),0,(struct sockaddr *)&addr_client,(socklen_t *)&len2);
		if(recv_num<0){
			perror("recvfrom error\n");
		}
		recv_buf[recv_num] =  '\0';
		printf("Hospital B has received input from client at location:%s\n", recv_buf);
		
		//find the shortest path
		int s;
		s = atoi(recv_buf);		
		map<int,float>result;
		int r_loc = path_finding(a,result,s);
		if(!r_loc){
			string nfd = "location not found";
			strcpy(send_buf,nfd.c_str());
            //reference: Beej's
			send_num = sendto(udp_sockfd, send_buf, 100,0,(struct sockaddr *)&addr_client, len);
			if(send_num<0){
				perror("sendto error\n");
				exit(1);
			}
			printf("Hospital B has sent 'location not found' to the Scheduler\n");
		}else{
			if(ava>=0 && ava <=1){
				cout << "Hospital B has capacity = " << cap_total << ",occupation= " << occupation <<", availability = " << ava <<endl;
			}
			dis = path_dis(result,a,s);
			float score;
            if(dis!=0){
                printf("Hospital B has found the shortest path to client,distance = %.2f\n",dis);
                //calulate
                score =(float) 1/(dis*ava);
                printf("Hospital B has the score = %.4f\n",score);
            }
			//send score and distance to scheduler
            //reference: Beej's
			stringstream strStream;
			strStream << score << " "<< dis ; 
			string sd = strStream.str();
			strcpy(send_buf,sd.c_str());
			send_num = sendto(udp_sockfd, send_buf, 100,0,(struct sockaddr *)&addr_client, len);
			if(send_num<0){
				perror("sendto error\n");
				exit(1);
			}
			if(dis == 0){
				printf("Hospital B has sent score = None and distance= None to the Scheduler\n");
			}else{
				printf("Hospital B has sent score = %.4f and distance= %.2f to the Scheduler\n",score,dis);
			}
            //receive assignment
            //reference: Beej's
			recv_num = recvfrom(udp_sockfd,recv_buf,sizeof(recv_buf),0,(struct sockaddr *)&addr_client,(socklen_t *)&len);
			if(recv_num<0){
				perror("recvfrom error\n");
			}
			recv_buf[recv_num] =  '\0';
			occupation += 1;
			ava = (float) (cap_total - occupation)/cap_total;
			cout << "Hospital B has been assigned to a client, occupation is updated to " << cap_total << ",occupation is updated to " << occupation <<", availability = " << ava <<endl;	
		}
        // block
		recv_num = recvfrom(udp_sockfd,recv_buf,sizeof(recv_buf),0,(struct sockaddr *)&addr_client,(socklen_t *)&len);
	}
	close(udp_sockfd);

    return 0;
}
