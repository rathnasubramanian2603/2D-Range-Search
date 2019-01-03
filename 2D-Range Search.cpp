#include<iostream>
#include <vector>
#include <windows.h>
#include<algorithm>
#include<math.h>
#include "GL/glut.h"
using namespace std;

struct point{
    int x;
    int y;
    point(int x,int y){
        this->x=x;
        this->y=y;
    }
};
vector<point> points;
vector<point> query;

bool compareX(point x,point y){
return x.x<y.x;
}

bool compareY(point x,point y){
return x.y<y.y;
}

void draw_line(point x1,point y1)
{
    glColor3f(1.0,0.0,0.0);
    glLineWidth(1);
    glBegin(GL_LINES);
        glVertex2f(x1.x,x1.y);
        glVertex2f(y1.x,y1.y);
    glEnd();
    glFlush();
}

class kdTree{

    public:
    kdTree *left;
    kdTree *right;
    point p;
    bool type;
    kdTree():left(0),right(0),p(0,0),type(false){
    }

};

void print_points(vector<point> p){
    for(int i=0;i<p.size();i++){
            cout<<p[i].x<<" "<<p[i].y<<endl;
    }
}

kdTree* build_kdTree(vector<point> P,int level){
    kdTree *root=new kdTree();
    cout<<"Number of Points "<<P.size()<<endl;
    if(P.size()>1){
        if(level%2==0){
            sort(P.begin(),P.end(),compareX);
            cout<<"Sort X"<<endl;
            print_points(P);
            root->type=true;
        }
        else{
            sort(P.begin(),P.end(),compareY);
            cout<<"Sort Y"<<endl;
            print_points(P);
            root->type=false;
        }
         int ind=ceil(P.size()/2);
         root->p=P[ind];
         cout<<"Median is "<<root->p.x<<" "<<root->p.y<<endl;
         vector<point> left(P.begin(),P.begin()+ind);
         vector<point> right(P.begin()+ind,P.end());
         root->left=build_kdTree(left,level+1);
         root->right=build_kdTree(right,level+1);
    }
    if(P.size()==1)
    {
        root->p.x=P[0].x;
        root->p.y=P[0].y;
    }
    return root;
}

void draw_kdTree(kdTree *tree,int x1,int x2,int y1,int y2){
    if(tree->left!=NULL || tree->right!=NULL){
        cout<<tree->p.x<<" "<<tree->p.y<<endl;
        if(tree->type==true){
                draw_line(point(tree->p.x,y1),point(tree->p.x,y2));
                draw_kdTree(tree->left,x1,tree->p.x,y1,y2);
                draw_kdTree(tree->right,tree->p.x,x2,y1,y2);
        }
        else{
            draw_line(point(x1,tree->p.y),point(x2,tree->p.y));
            draw_kdTree(tree->left,x1,x2,y1,tree->p.y);
            draw_kdTree(tree->right,x1,x2,tree->p.y,y2);
        }
    }
}
kdTree *KDtree;
void start()
{
    print_points(points);
    KDtree=build_kdTree(points,0);
    cout<<"Successful"<<endl;
    draw_kdTree(KDtree,0,640,0,480);
   // draw_line(points[0],points[1]);
}
void plot_point(int x,int y){

    glPointSize(5);
    glBegin(GL_POINTS);
        glVertex2f(x,y);
    glEnd();
    glFlush();
}
void report_subtree(kdTree *tree){

    if(tree->left!=NULL){
        report_subtree(tree->left);
    }
    if(tree->right!=NULL){
        report_subtree(tree->right);
    }
    if(tree->left==NULL && tree->right==NULL){
        glColor3f(1,0,0);
        plot_point(tree->p.x,tree->p.y);
    }
}
void compute_query(kdTree *tree,point q1,point q2,point r1,point r2){

    if(tree->left==NULL && tree->right==NULL){
        point p=tree->p;
        if(p.x>=q1.x && p.x <=q2.x && p.y>=q1.y && p.y<=q2.y){
            glColor3f(1,0,0);
            plot_point(p.x,p.y);
        }

    }
    else{
        point p=tree->p;
        if(tree->type==true){
            if(q1.x<=r1.x && q2.x>=p.x && q1.y<=r1.y && q2.y>=r2.y){
                report_subtree(tree->left);
            }
            else{
                compute_query(tree->left,q1,q2,r1,point(p.x,r2.y));
            }
            if(q1.x<=p.x && q2.x>=r2.x && q1.y<=r1.y && q2.y>=r2.y){
                report_subtree(tree->right);
            }
            else{
                compute_query(tree->right,q1,q2,point(p.x,r1.y),r2);
            }
        }
        else if(tree->type==false){
            if(q1.x<=r1.x && q2.x>=r2.x && q1.y<=r1.y && q2.y>=p.y){
                report_subtree(tree->right);
            }
            else{
                compute_query(tree->left,q1,q2,r1,point(r2.x,p.y));
            }
            if(q1.x<=r1.x && q2.x>=r2.x && q1.y<=p.y && q2.y>=r2.y){
                report_subtree(tree->right);
            }
            else{
                compute_query(tree->right,q1,q2,point(r1.x,p.y),r2);
            }

        }
    }
}

void init(){
    glClearColor(1,1,1,0); // JMU Gold
     // JMU Purple
    glMatrixMode(GL_PROJECTION);
     gluOrtho2D(0,640,0,480);
}

static void display(){
   // glClear(GL_COLOR_BUFFER_BIT);
   // plot_point(100,100);
    glFlush();
}

void plotrect()
{
    glBegin(GL_LINE_LOOP);
        glVertex2f(query[0].x,query[0].y);
        glVertex2f(query[0].x,query[1].y);
        glVertex2f(query[1].x,query[1].y);
        glVertex2f(query[1].x,query[0].y);
    glEnd();
    glFlush();
}

void mouse_plot(int button,int state,int x,int y){

    static bool flag=false;
    static int c=0;
    if(button== GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        y=480-y;
        if(flag==false){
        glColor3f(0.0,1.0,0.0);
        points.push_back(point(x,y));
        plot_point(x,y);
        }
        else if(flag==true)
        {
            c++;
            query.push_back(point(x,y));
            if(c<=2)
            {
                glColor3f(0.0,0.0,1.0);
                plot_point(x,y);

            }
            if(c==2)
            {
                plotrect();
                compute_query(KDtree,query[0],query[1],point(0,0),point(640,480));
            }
        }

    }
    else if(button== GLUT_RIGHT_BUTTON && state==GLUT_DOWN)
    {
        flag=true;
        start();


    }
}


int main(int argc,char **argv){

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Test");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse_plot);
    glutMainLoop();
    //return 0;
}
