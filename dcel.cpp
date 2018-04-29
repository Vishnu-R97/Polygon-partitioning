#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>
using namespace std;

struct half_edge;
struct face;

struct vertex{//structure to define a point,x and y coordinates
    int id;
    float x;
    float y;
    int vertex_type;
    half_edge *incident;

};

struct half_edge{
    int edge_id;
    vertex *origin;
    vertex *next;
    half_edge *next_edge;
    half_edge *previous_edge;
    half_edge *twin;
    face *incident_face;
};

struct face{
    int face_id;
    half_edge* start;
};

vector <vertex*> v_record;int v_count = 0;
vector <half_edge*> e_record;int e_count = 0;
vector <face*> f_record;int f_count = 0;

//add one vertex to your set of points,along with an incident edge
void add_vertex(float x,float y){
    vertex* p = new vertex;
    p->id = ++v_count;
    p->x = x;
    p->y = y;
    //p.incident = &incident;
    v_record.push_back(p);
    /*cout << "------------------------\nVERTICES\n";
    int i;
    for(i=0;i<v_count;i++){

        cout << "Vertex id:" << v_record[i]->id << " | coordinates:" << v_record[i]->x << "," << v_record[i]->y << "\n";
    }*/

}

//Base case to add edge
void add_edge(vertex* v1,vertex* v2){
    half_edge* e1 = new half_edge;
    e1->edge_id = ++e_count;
    e1->origin = v1;
    e1->next = v2;
    e_record.push_back(e1);
    v1->incident = e_record[e_count-1];
    int i;
    /*cout << "\nCURRENT EDGES COUNT:" << e_count << "\n";
    for(i = 0;i < e_count ; i++){
        cout<< "\t" << e_record[i]->origin->x << "," << e_record[i]->origin->y << "--->" << e_record[i]->next->x << "," << e_record[i]->next->y << "\n";
    }*/
}

//add an edge that already has a twin setup
void add_edge_and_twin_setup(vertex* v1,vertex* v2,half_edge* twin,face* incident_face){
    half_edge* e1 = new half_edge;
    e1->edge_id = ++e_count;
    e1->origin = v1;
    e1->next = v2;
    e1->twin = twin;
    e1->incident_face = incident_face;
    //cout << e1->edge_id <<" "<< e1->origin->x <<" "<<e1->origin->y<<endl;
    e_record.push_back(e1);
    twin->twin = e_record[e_count-1];

    int i;
    /*cout << "\nCURRENT EDGES COUNT:" << e_count << "\n";
    for(i = 0;i < e_count ; i++){
        cout<< "\t" << e_record[i]->origin->x << "," << e_record[i]->origin->y << "--->" << e_record[i]->next->x << "," << e_record[i]->next->y << "\n";
    }*/

}


//add an edge with previous present
void add_edge_and_previous_setup(vertex* v1,vertex* v2,half_edge* previous,face* incident_face){
    half_edge* e1 = new half_edge;
    e1->edge_id = ++e_count;
    e1->origin = v1;
    e1->next = v2;
    e1->previous_edge = previous;
    e1->incident_face = incident_face;
    e_record.push_back(e1);
    v1->incident = e_record[e_count-1];
    previous->next_edge = e_record[e_count-1];

    int i;


}

void add_face(half_edge* start){
    face* f = new face;
    f->face_id = ++f_count;
    f->start = start;
    f_record.push_back(f);

    start->incident_face = f_record[f_count-1];

}

void split(vertex* v1,vertex* v2){

    int i;
    int found = 0;
    int face=0;
    //find the face
    for(i=0;i<f_count;i++){
        if(found==1){

            break;
        }
        half_edge* s = f_record[i]->start;
        half_edge* t = s;
        int visited = 0;
        //starting case
        if(t->origin->x == v1->x && t->origin->y == v1->y){
            visited++;
        }
        if(t->origin->x == v2->x && t->origin->y == v2->y){
            visited++;
        }

        t= t->next_edge;

        //iterative case
        while(t->edge_id!=s->edge_id){

            if(visited==2){
                found = 1;
                face = i;
                break;
            }
            if(t->origin->x == v1->x && t->origin->y == v1->y)
                visited++;
            if(t->origin->x == v2->x && t->origin->y == v2->y)
                visited++;

            
            t = t->next_edge;
        }
    }

    half_edge* s = f_record[face]->start;
    half_edge* t = s;
    int points_visited = 0;
    int new_face_index ;
    int first_edge_index,second_edge_index,first_edge_next_index,second_edge_next_index;
    //starting
    if(  (t->origin->x == v1->x && t->origin->y == v1->y)  && points_visited == 0 ){
        add_edge_and_previous_setup(v1,v2,t->previous_edge,t->previous_edge->incident_face);
        //cout<<"\t\t\t\tcase_0,v1::edge created from "<<v1->id<<"to "<<v2->id<<endl;
        first_edge_index = e_count-1;
        second_edge_next_index = t->edge_id-1;
        f_record[face]->start = t->previous_edge;
        add_face(t);
        new_face_index = f_count-1;
        points_visited++;

    }
    if(  (t->origin->x == v2->x && t->origin->y == v2->y)  && points_visited == 0 ){
        add_edge_and_previous_setup(v2,v1,t->previous_edge,t->previous_edge->incident_face);
        //cout<<"\t\t\t\tcase_0,v2::edge created from "<<v2->id<<"to "<<v1->id<<endl;
        first_edge_index = e_count-1;
        second_edge_next_index = t->edge_id-1;
        f_record[face]->start = t->previous_edge;
        add_face(t);
        new_face_index = f_count-1;
        points_visited++;


    }

    t = t->next_edge;
    
    //iterative
    while(t->edge_id != s->edge_id){
        
        if(points_visited==2){
            break;
        }
        
        if(points_visited==1){
            
            t->incident_face = f_record[new_face_index];
        }
        
        if(  (t->origin->x == v1->x && t->origin->y == v1->y)  && points_visited == 0 ){
            add_edge_and_previous_setup(v1,v2,t->previous_edge,t->previous_edge->incident_face);
            //cout<<"\t\t\t\tcase_0,v1::edge created from "<<v1->id<<"to "<<v2->id<<endl;
            first_edge_index = e_count-1;
            second_edge_next_index = t->edge_id-1;
            f_record[face]->start = t->previous_edge;
            add_face(t);
            new_face_index = f_count-1;
            points_visited++;
            t = t->next_edge;

            continue;

        }
        
        if(  (t->origin->x == v2->x && t->origin->y == v2->y)  && points_visited == 0 ){
            add_edge_and_previous_setup(v2,v1,t->previous_edge,t->previous_edge->incident_face);
            //cout<<"\t\t\t\tcase_0,v2::edge created from "<<v2->id<<"to "<<v1->id<<endl;
            first_edge_index = e_count-1;
            second_edge_next_index = t->edge_id-1;
            f_record[face]->start = t->previous_edge;
            add_face(t);
            new_face_index = f_count-1;
            points_visited++;
            t = t->next_edge;
            
            continue;

        }

        if(  (t->origin->x == v1->x && t->origin->y == v1->y)  && points_visited == 1 ){
            add_edge_and_previous_setup(v1,v2,t->previous_edge,t->previous_edge->incident_face);
            //cout<<"\t\t\t\tcase_1,v1::edge created from "<<v1->id<<"to "<<v2->id<<endl;
            second_edge_index = e_count-1;
            first_edge_next_index = t->edge_id-1;
            
            points_visited++;
            t = t->next_edge;
            continue;

        }
        if(  (t->origin->x == v2->x && t->origin->y == v2->y)  && points_visited == 1 ){
            add_edge_and_previous_setup(v2,v1,t->previous_edge,t->previous_edge->incident_face);
            //cout<<"\t\t\t\tcase_1,v2::edge created from "<<v2->id<<"to "<<v1->id<<endl;
            second_edge_index = e_count-1;
            first_edge_next_index = t->edge_id-1;
            points_visited++;
            t = t->next_edge;
            continue;

        }


        t = t->next_edge;
        
    }
    e_record[first_edge_index]->twin = e_record[second_edge_index];
    e_record[first_edge_index]->twin = e_record[first_edge_index];
    e_record[first_edge_index]->next_edge = e_record[first_edge_next_index];
    e_record[first_edge_next_index]->previous_edge = e_record[first_edge_index];
    e_record[second_edge_index]->next_edge = e_record[second_edge_next_index];
    e_record[second_edge_next_index]->previous_edge = e_record[second_edge_index];
    //cout<<"\n--------------\n"<<"\t\t\tsplit over.Joined "<<v1->id<<" to "<<v2->id <<":new face id: "<< f_record[f_count-1]->face_id <<"\n-------------\n";


}



//SEARCH TREE STRUCTURE
vector <int> J_edges;//stores edge id of all edges in a vector

void add_J_edge(int edge_id){
    J_edges.push_back(edge_id);
    /*cout<<"\n\t\t\t\t\t\t\t\t";
    for(int i=0;i<J_edges.size();i++){
        cout<<J_edges[i]<<" ";
    }
    cout<<"\n\n";*/
}

void remove_J_edge(int edge_id){
    
    int i;
    int found = 0;
    for(i=0;i<J_edges.size();i++){
        if(J_edges[i] == edge_id){
            
            J_edges.erase(J_edges.begin() + i);
        }
    }
    /*cout<<"\n\t\t\t\t\t\t\t\t";
    for(int i=0;i<J_edges.size();i++){
        cout<<J_edges[i]<<" ";
    }
    cout<<"\n\n";
    */
}


double turn(vertex* p1,vertex* p2,vertex* p3){
    double ans = 0;
    ans += p1->x * (p2->y -p3->y);
    ans += p2->x * (p3->y -p1->y);
    ans += p3->x * (p1->y -p2->y);
    return ans;
}

void handle_1(int vertex_id,int helper[]){//start vertex
    //cout<<"entered 1"<<endl;
    int edge_i = v_record[vertex_id-1]->incident->edge_id - 1;
    add_J_edge(edge_i+1);
    helper[edge_i] = vertex_id;
    //cout<<"\tHelper added to index "<<edge_i<<": "<<helper[edge_i]<<endl;
}


//deletes existing J tree and returns new J tree
void handle_2(int vertex_id,int helper[]){//end vertex
    //cout<<"entered 2"<<endl;
    int edge_i_minus = v_record[vertex_id-1]->incident->previous_edge->edge_id - 1;
    if(helper[edge_i_minus]>0){
        if( v_record[ helper[edge_i_minus] - 1 ]->vertex_type == 5){

            split(v_record[vertex_id-1],v_record[ helper[edge_i_minus] - 1 ] );

        }
    }
    remove_J_edge(edge_i_minus+1);
    
}



half_edge* find_best_left(vertex* v){//returns edge_id
    int i;
    half_edge* ans = new half_edge;
    ans->edge_id = -1;
    vector <int> left;
    for(i=0;i<J_edges.size();i++){
        if( e_record[ J_edges[i] - 1 ]->origin->x <= v->x && e_record[ J_edges[i] - 1 ]->next->x <= v->x  ){//it is to the left of point
            if( e_record[ J_edges[i] - 1 ]->origin->y >= v->y && e_record[ J_edges[i] - 1 ]->next->y <= v->y  ){
                left.push_back( J_edges[i]  );
            }
            if( e_record[ J_edges[i] - 1 ]->origin->y <= v->y && e_record[ J_edges[i] - 1 ]->next->y >= v->y  ){
                left.push_back( J_edges[i]  );
            }

        }
    }

    if(left.size()==0){return ans;}
    int left_max = left[0] ;
    if(left.size()>1){
        for(i=0;i<left.size();i++){
            if( e_record[ left[i]-1 ]->origin->x > e_record[ left_max - 1 ]->origin->x ){
                left_max = left[i];
            }
        }
    }

    return e_record[left_max-1];
}





void handle_3(int vertex_id,int helper[]){//regular vertex
    vertex* v = v_record[ vertex_id-1 ];
    //cout<<"entered 3"<<endl;
    if(v->incident->previous_edge->origin->y > v->y){//P to the right case
        half_edge* edge_i = v->incident;
        half_edge* edge_i_minus = v->incident->previous_edge;
        if(helper[ edge_i_minus->edge_id - 1 ]>0){
            if(v_record[ helper[ edge_i_minus->edge_id - 1 ] - 1 ]->vertex_type==5){

                split(v,v_record[ helper[ edge_i_minus->edge_id - 1 ]-1 ]);

                remove_J_edge(edge_i_minus->edge_id);
                
                add_J_edge(v->incident->edge_id);
                
                helper[v->incident->edge_id-1] = vertex_id;
                //cout<<"\tHelper added to index "<<v->incident->edge_id-1<<": "<<helper[v->incident->edge_id-1]<<endl;
            }
        }

        else{
            //cout<<"\tElse case"<<endl;
            half_edge* edge = find_best_left(v);
            if(edge->edge_id!=-1){
                //cout<<"\tFound best left,edge id:"<<edge->edge_id<<endl;
                if(helper[edge->edge_id-1]>0){
                    if(v_record[ helper[edge->edge_id-1] -1]->vertex_type==5){
                        split(v,v_record[ helper[edge->edge_id-1] -1 ]);
                        helper[edge->edge_id-1] = v->id;
                        //cout<<"\tHelper added to index "<<edge->edge_id - 1<<": "<<helper[edge->edge_id - 1]<<endl;
                    }
                }
            }
         }

    }

}



void handle_4(int vertex_id,int helper[]){//split vertex
    //cout<<"entered 4"<<endl;
    vertex* v = v_record[vertex_id-1];
    //search J and find edge directly left of vertex
    half_edge* edge = find_best_left(v);
    if(edge->edge_id!=-1){
        //cout<<"\tFound best left,edge id:"<<edge->edge_id<<endl;
        if(helper[edge->edge_id-1]>0){
            split(v,v_record[ helper[edge->edge_id - 1] -1 ]);
            helper[edge->edge_id - 1] = v_record[vertex_id -1]->id;
            //cout<<"\tHelper added to index "<<edge->edge_id - 1<<": "<<helper[edge->edge_id - 1]<<endl;
        }
    }
    add_J_edge( v->incident->edge_id );
    
    helper[ v->incident->edge_id - 1 ] = vertex_id;
    //cout<<"\tHelper added to index "<<v->incident->edge_id - 1<<": "<<helper[v->incident->edge_id - 1]<<endl;
}

void handle_5(int vertex_id,int helper[]){//merge vertex
    half_edge* edge_i_minus = v_record[vertex_id-1]->incident->previous_edge;
    //cout<<"entered 5"<<endl;
    if(helper[edge_i_minus->edge_id-1]>0){
        if( v_record[ helper[edge_i_minus->edge_id - 1] - 1 ]->vertex_type == 5 ){
            
            split(v_record[vertex_id - 1],v_record[ helper[ edge_i_minus->edge_id - 1 ]-1 ]);
        }
    }
    remove_J_edge(edge_i_minus->edge_id);
    half_edge* e_j = find_best_left(v_record[vertex_id-1]);
    
    if(e_j->edge_id!=-1){

        //cout<<"\tFound best left,edge id:"<<e_j->edge_id<<endl;
        if(v_record[helper[e_j->edge_id-1] -1 ]->vertex_type==5){
            split(v_record[vertex_id - 1], v_record[ helper[e_j->edge_id - 1] -1 ] );
        }
        helper[e_j->edge_id-1] = vertex_id;
        //cout<<"\tHelper added to index "<<e_j->edge_id - 1<<": "<<helper[e_j->edge_id - 1]<<endl;
    }
}



bool sortcol( const vector<int>& v1,
               const vector<int>& v2 ) {
 return (  v_record[v1[0]-1 ]->y < v_record[v2[0]-1 ]->y || ( v_record[v1[0]-1 ]->y == v_record[v2[0]-1 ]->y && v_record[v1[0]-1 ]->x > v_record[v2[0]-1 ]->x  )  );
}


void y_monotone(){//partitions the DCEL structure into y_monotone polygons
    int i;
    half_edge* start = f_record[0]->start;

    //1:start vertex|||| neighbour pts are below and 3 pts in order make left turn
    //2:end vertex|||| neighbour pts are above and 3 pts in order make left turn
    //3:regular vertex|||| normal vertex going top->bttm or b->t
    //4:split vertex|||| neighbour pts are below and 3 pts in order make right turn
    //5:merge vertex|||| neighbour pts are above and 3 pts in order make right turn

    vector <vector <int> > q;//index 0: id,index 1:type_flag
    vector <int> temp;

    half_edge* current = start;
    vertex *s = start->origin;
    vertex *t = start->origin;
    vertex *t_prev = start->previous_edge->origin;
    vertex *t_next = start->next;
    temp.push_back(t->id);
    if( ( (t_prev->y > t->y)||(t_prev->y==t->y && t_prev->x < t->x) )&&
        (  (t_next->y > t->y)||(t_next->y==t->y && t_next->x < t->x) )  ){
        //can be case 2 or 5
        if(turn(t_prev,t,t_next)>0){//left turn case
            temp.push_back(2);
            v_record[temp[0]-1]->vertex_type = 2;

        }
        if(turn(t_prev,t,t_next)<0){//right turn case
            temp.push_back(5);
            v_record[temp[0]-1]->vertex_type = 5;

        }
    }

    else if( ( (t_prev->y < t->y)||(t_prev->y==t->y && t_prev->x > t->x) )&&
             ( (t_next->y < t->y)||(t_next->y==t->y && t_next->x > t->x) ) ){
        //can be case 1 or 4
        if(turn(t_prev,t,t_next)>0){//left turn case
            temp.push_back(1);
            v_record[temp[0]-1]->vertex_type = 1;
        }
        if(turn(t_prev,t,t_next)<0){//right turn case
            temp.push_back(4);
            v_record[temp[0]-1]->vertex_type = 4;
        }

    }
    else{//case 3
        temp.push_back(3);
        v_record[temp[0]-1]->vertex_type = 3;
    }
    q.push_back(temp);
    temp.clear();
    current = start->next_edge;

    while(current->edge_id!=start->edge_id){

        t = current->origin;
        t_next = current->next;
        t_prev = current->previous_edge->origin;

        temp.push_back(t->id);
        if( ( (t_prev->y > t->y)||(t_prev->y==t->y && t_prev->x < t->x) )&&
        (  (t_next->y > t->y)||(t_next->y==t->y && t_next->x < t->x) )  ){
        //can be case 2 or 5
            if(turn(t_prev,t,t_next)>0){//left turn case
                temp.push_back(2);
                v_record[temp[0]-1]->vertex_type = 2;
            }
            if(turn(t_prev,t,t_next)<0){//right turn case
                temp.push_back(5);
                v_record[temp[0]-1]->vertex_type = 5;
            }
        }

        else if( ( (t_prev->y < t->y)||(t_prev->y==t->y && t_prev->x > t->x) )&&
                 ( (t_next->y < t->y)||(t_next->y==t->y && t_next->x > t->x) ) ){
            //can be case 1 or 4
            if(turn(t_prev,t,t_next)>0){//left turn case
                temp.push_back(1);
                v_record[temp[0]-1]->vertex_type = 1;
            }
            if(turn(t_prev,t,t_next)<0){//right turn case
                temp.push_back(4);
                v_record[temp[0]-1]->vertex_type = 4;
            }

        }
        else{
            //case 3
            temp.push_back(3);
            v_record[temp[0]-1]->vertex_type = 3;
        }
        //cout<< "Current edge id:" << current->edge_id << endl;
        q.push_back(temp);
        temp.clear();
        current = current->next_edge;

    }


    sort(q.begin(),q.end(),sortcol);
    int helper[e_count];//each index is edge_id -1,each value is vertex index
    memset(helper,0,e_count*sizeof(int));

    for(i=q.size()-1;i>=0;i--){
        
        if(q[i][1]==1){
            handle_1(q[i][0],helper);
        }
        if(q[i][1]==2){
            handle_2(q[i][0],helper);
        }
        if(q[i][1]==3){
            handle_3(q[i][0],helper);
        }
        if(q[i][1]==4){
            handle_4(q[i][0],helper);
        }
        if(q[i][1]==5){
            handle_5(q[i][0],helper);
        }
    }


    cout<< endl;

}

vector <int> stack;


int face_iterate(int face_index){
    face* f = f_record[face_index];
    int ans = 0;
    half_edge* s = f->start;
    half_edge* t = s->next_edge;
    ans++;
    ans++;
    t= t->next_edge;
    while(t->edge_id!=s->edge_id){
        ans++;        
        t = t->next_edge;
    }
    cout<<endl;
    return ans;
}




void triangulate(face* f){
    if(face_iterate(f->face_id-1)==3)
        return;
    half_edge* s = f->start;
    half_edge* t = s;
    int i,j;
    int previous_point;
    //cout<<"face starts at:"<<t->origin->id<<" to "<<t->next->id<<endl;
    int top_point,top_edge,bottom_point,bottom_edge;
    top_point = t->origin->id - 1;
    top_edge = t->edge_id;
    bottom_point = t->origin->id - 1;
    bottom_edge = t->edge_id;
    t = t->next_edge;

    while(t->edge_id != s->edge_id){
        
        if(t->origin->y > v_record[top_point]->y || ( t->origin->y == v_record[top_point]->y && t->origin->x < v_record[top_point]->x)){
            top_point = t->origin->id - 1;
            top_edge = t->edge_id - 1;
        }
        if(t->origin->y < v_record[bottom_point]->y || ( t->origin->y == v_record[bottom_point]->y && t->origin->x > v_record[bottom_point]->x)){
            bottom_point = t->origin->id - 1 ;
            bottom_edge = t->edge_id - 1;
        }
        t = t->next_edge;
    }
    v_record[top_point]->incident = e_record[top_edge];
    v_record[bottom_point]->incident = e_record[bottom_edge];
    half_edge* left = v_record[top_point]->incident;
    half_edge* right = v_record[top_point]->incident->previous_edge;
    vector <int> q;//contains vertex id's
    
    while( left->edge_id != right->edge_id ){
        if(left->origin->y > right->origin->y || (left->origin->y == right->origin->y && left->origin->x < right->origin->x) ){
            q.push_back(left->origin->id);
            left->origin->vertex_type = 0;
            left = left->next_edge;
            continue;
        }
        if(left->origin->y < right->origin->y || (left->origin->y == right->origin->y && left->origin->x > right->origin->x) ){
            q.push_back(right->origin->id);
            right->origin->vertex_type = 1;
            right = right->previous_edge;
            continue;
        }

    }
    q.push_back(v_record[bottom_point]->id);
    right->origin->vertex_type = 1;
    cout<<endl;
    vector <int> stack;//stack stores vertex ids.
    int z;
    int recent;
    stack.push_back(q[0]);
    stack.push_back(q[1]);
    for(i=2;i<q.size()-1;i++){
        if(v_record[ q[i]-1 ]->vertex_type != v_record[ stack[stack.size()-1]-1 ]->vertex_type ){
            if(stack.size()>0){
                for(j=stack.size()-1;j>0;j--){
                    split(v_record[ q[i]-1 ],v_record[ stack[ j]-1 ]);
                    stack.pop_back();
                }
                stack.pop_back();//all elements popped out of stack and handled
            }
    
            stack.push_back(q[i-1]);
            stack.push_back(q[i]);
        }
        else if(v_record[ q[i]-1 ]->vertex_type == v_record[ stack[stack.size()-1]-1 ]->vertex_type){
            recent = stack[ stack.size()-1 ];
            stack.pop_back();
            while(v_record[ q[i]-1 ]->vertex_type != v_record[ stack[stack.size()-1]-1 ]->vertex_type && stack.size()> 0 ){
                recent = stack[ stack.size()-1 ];
                split(v_record[q[i]-1],v_record[ recent-1 ]);
                if(stack.size()>0)
                    stack.pop_back();
                cout<<endl;
                if(stack.size()==0)
                    break;
            }
            stack.push_back(recent);
            stack.push_back(q[i]);
        }
    }
    if(stack.size()>0){stack.pop_back();}
    for(j=stack.size()-1;j>0;j--){
        split(v_record[ q[i]-1 ],v_record[ stack[ j]-1 ]);
        stack.pop_back();
    }
    if(stack.size()==1)
        stack.pop_back();
    

}









int main(){
    //set up a face for all outer edges
    face outside;outside.face_id = -99;
    //Below is a DCEL with inner edges in counter clockwise
    int i;
    add_vertex(1,2);//id = 1,thus index = 0
    add_vertex(3,1);//id = 2,thus index = 1
    add_edge(v_record[0],v_record[1]);//edge id = 1
    add_face(e_record[0]);//face id = 1
    add_edge_and_twin_setup(v_record[1],v_record[0],e_record[0],&outside);//edge id = 2
    add_vertex(5,2);//id = 3
    add_edge_and_previous_setup(v_record[1],v_record[2],e_record[0],f_record[0]);//edge id = 3
    add_edge_and_twin_setup(v_record[2],v_record[1],e_record[2],&outside);//edge id = 4
    add_vertex(6,1);//id = 4
    add_edge_and_previous_setup(v_record[2],v_record[3],e_record[2],f_record[0]);//edge id = 5-----------------
    add_edge_and_twin_setup(v_record[3],v_record[2],e_record[4],&outside);//edge id = 6
    add_vertex(6,2);//id = 5
    add_edge_and_previous_setup(v_record[3],v_record[4],e_record[4],f_record[0]);//edge id = 7
    add_edge_and_twin_setup(v_record[4],v_record[3],e_record[6],&outside);//edge id = 8
    add_vertex(7,2);//id = 6
    add_edge_and_previous_setup(v_record[4],v_record[5],e_record[6],f_record[0]);//edge id  = 9
    add_edge_and_twin_setup(v_record[5],v_record[4],e_record[8],&outside);//edge id = 10
    add_vertex(8,4);//id = 7
    add_edge_and_previous_setup(v_record[5],v_record[6],e_record[8],f_record[0]);//edge id = 11
    add_edge_and_twin_setup(v_record[6],v_record[5],e_record[10],&outside);//edge id = 12
    add_vertex(7,6);//id = 8
    add_edge_and_previous_setup(v_record[6],v_record[7],e_record[10],f_record[0]);//edge id = 13
    add_edge_and_twin_setup(v_record[7],v_record[6],e_record[12],&outside);//edge id = 14
    add_vertex(7,9);//id = 9
    add_edge_and_previous_setup(v_record[7],v_record[8],e_record[12],f_record[0]);//edge id = 15
    add_edge_and_twin_setup(v_record[8],v_record[7],e_record[14],&outside);//edge id = 16
    add_vertex(5,8);//id = 10
    add_edge_and_previous_setup(v_record[8],v_record[9],e_record[14],f_record[0]);//edge id = 17
    add_edge_and_twin_setup(v_record[9],v_record[8],e_record[16],&outside);//edge id = 18
    add_vertex(3,9);//id = 11
    add_edge_and_previous_setup(v_record[9],v_record[10],e_record[16],f_record[0]);//edge id = 19
    add_edge_and_twin_setup(v_record[10],v_record[9],e_record[18],&outside);//edge id = 20
    add_vertex(2,7);//id = 12
    add_edge_and_previous_setup(v_record[10],v_record[11],e_record[18],f_record[0]);//edge id = 21
    add_edge_and_twin_setup(v_record[11],v_record[10],e_record[20],&outside);//edge id = 22
    add_vertex(4,6);//id = 13
    add_edge_and_previous_setup(v_record[11],v_record[12],e_record[20],f_record[0]);//edge id = 23
    add_edge_and_twin_setup(v_record[12],v_record[11],e_record[22],&outside);//edge id = 24
    add_vertex(3,4);//id = 14
    add_edge_and_previous_setup(v_record[12],v_record[13],e_record[22],f_record[0]);//edge id = 25
    add_edge_and_twin_setup(v_record[13],v_record[12],e_record[24],&outside);//edge id = 26
    add_vertex(2,5);//id = 15
    add_edge_and_previous_setup(v_record[13],v_record[14],e_record[24],f_record[0]);//edge id = 27
    add_edge_and_twin_setup(v_record[14],v_record[13],e_record[26],&outside);//edge id = 28
    add_edge_and_previous_setup(v_record[14],v_record[0],e_record[26],f_record[0]);//edge id = 29
    add_edge_and_twin_setup(v_record[0],v_record[14],e_record[28],&outside);//edge od = 30
    e_record[0]->previous_edge = e_record[28];
    e_record[28]->next_edge = e_record[0];
    //print faces
    cout << "------------------------\nFACES\n";
    for(i=0;i<f_count;i++){
        cout << "face id:" << f_record[i]->face_id << " | starting edge:" << f_record[i]->start->origin->x << "," << f_record[i]->start->origin->y  << " to " << f_record[i]->start->next->x << "," << f_record[i]->start->next->y << "\n";
    }
    cout << "------------------------\nEDGES\n";
    //print edges
    for(i=0;i<e_count;i++){
        cout<< "\tEdge id = " << e_record[i]->edge_id<< "\tstart = " << e_record[i]->origin->x << "," << e_record[i]->origin->y << " | next = " << e_record[i]->next->x << "," << e_record[i]->next->y << "\n";
    }
    cout << "------------------------\nVERTICES\n";
    for(i=0;i<v_count;i++){
        cout << "\t\tVertex id:" << v_record[i]->id << " | coordinates:" << v_record[i]->x << "," << v_record[i]->y << "\n";
    }

    if(v_count>0 && e_count>0 && f_count>0)
        cout<<"Done.Success.\n";

    y_monotone();

    cout<<"Result of y monotone:\n\n\n";

    cout << "------------------------\nFACES\n";
    for(i=0;i<f_count;i++){
        cout << "face id:" << f_record[i]->face_id << " | starting edge:" << f_record[i]->start->origin->x << "," << f_record[i]->start->origin->y  << " to " << f_record[i]->start->next->x << "," << f_record[i]->start->next->y << "\n";
        
    }
    cout << "------------------------\nEDGES\n";
    //print edges
    for(i=0;i<e_count;i++){
        cout<< "\tEdge id = " << e_record[i]->edge_id<< "\tstart = " << e_record[i]->origin->x << "," << e_record[i]->origin->y << " | next = " << e_record[i]->next->x << "," << e_record[i]->next->y << "\n";
    }
    cout << "------------------------\nVERTICES\n";
    for(i=0;i<v_count;i++){
        cout << "\t\tVertex id:" << v_record[i]->id << " | coordinates:" << v_record[i]->x << "," << v_record[i]->y << "\n";
    }

    if(v_count>0 && e_count>0 && f_count>0)
        cout<<"Done.Success.\n";

    for(i=0;i<f_count;i++){
        triangulate(f_record[i]);
    }

    cout<<"Result of triangulate:\n\n\n";

    cout << "------------------------\nFACES\n";
    for(i=0;i<f_count;i++){
        cout << "face id:" << f_record[i]->face_id << " | starting edge:" << f_record[i]->start->origin->x << "," << f_record[i]->start->origin->y  << " to " << f_record[i]->start->next->x << "," << f_record[i]->start->next->y << "\n";
        
    }
    cout << "------------------------\nEDGES\n";
    //print edges
    for(i=0;i<e_count;i++){
        cout<< "\tEdge id = " << e_record[i]->edge_id<< "\tstart = " << e_record[i]->origin->x << "," << e_record[i]->origin->y << " | next = " << e_record[i]->next->x << "," << e_record[i]->next->y << "\n";
    }
    cout << "------------------------\nVERTICES\n";
    for(i=0;i<v_count;i++){
        cout << "\t\tVertex id:" << v_record[i]->id << " | coordinates:" << v_record[i]->x << "," << v_record[i]->y << "\n";
    }

    if(v_count>0 && e_count>0 && f_count>0)
        cout<<"Done.Success.\n";

    


    return 0;
}



