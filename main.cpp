#include<vector>
#include<stdio.h>
#include<math.h>
#include<ctime>
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include<fstream>
using namespace std;
#define TIME_LIMIT 86400
#define LAMBDA 2/60
#define AVG_P -110
typedef struct{
    float x;
    float y;
    int pold[4]; // store p old id 0~3
    int en_num;
    int dir;
    /*  0: up
        1: right
        2: down
        3: left
    */
}car;
void geneCar(vector<car>&,vector<vector<int> >,float);
void moveCar(vector<car>&,vector<vector <int> >);
void checkBound(vector<car>&ref_car);
float dist(float,float,float,float);
float getP(float,float);
int n = 0;
double poffset1 = 0;
double poffset2 = 0;
double poffset3 = 0;
double poffset4 = 0;
int handOffPolicy(vector<car>&,vector<vector<int> >,int);

int main(){
    vector<car> carv;
    const vector<vector <int> > entry{{750,0,2},{1500,0,2},{2250,0,2},
                      {0,750,1},{0,1500,1},{0,2250,1},
                      {750,3000,0},{1500,3000,0},{2250,3000,0},
                      {3000,750,3},{3000,1500,3},{3000,2250,3}};
    const vector<vector <int> > base{{750,750},
                                     {2250,750},
                                     {750,2250},
                                     {2250,2250}};
    const vector<vector <int> > corner{{750,750},{750,1500},{750,2250},
                                        {1500,750},{1500,1500},{1500,2250},
                                        {2250,750},{2250,1500},{2250,2250}};
    vector<float> policy1pold;
    int curtime = 0;
    int poli1_handoff_num = 0;
    int poli2_handoff_num = 0;
    int poli3_handoff_num = 0;
    float poiP = exp(-LAMBDA)*LAMBDA;
    //　file manipulation
	ofstream ifso1;
    ofstream ifso2;
    ofstream ifso3;
	ifso1.open("policy1.txt",ios::out|ios::trunc);
    ifso2.open("policy2.txt",ios::out|ios::trunc);
    ifso3.open("policy3.txt",ios::out|ios::trunc);

    srand(time(NULL));
    while(curtime<TIME_LIMIT){

        geneCar(carv,entry,poiP);
        moveCar(carv,corner);
        checkBound(carv);
        poli1_handoff_num += handOffPolicy(carv,base,1);
        poli2_handoff_num += handOffPolicy(carv,base,2);
        poli3_handoff_num += handOffPolicy(carv,base,3);
        curtime++;

        ifso1<<poli1_handoff_num<<endl;
        ifso2<<poli2_handoff_num<<endl;
        ifso3<<poli3_handoff_num<<endl;
    }
    cout<<carv.size()<<endl;
    ifso1.close();
    ifso2.close();
    ifso3.close();
    cout<<"Average P in policy 1 is: "<<(double)AVG_P+poffset1/TIME_LIMIT<<" dbm"<<endl;
    cout<<"Average P in policy 2 is: "<<(double)AVG_P+poffset2/TIME_LIMIT<<" dbm"<<endl;
    cout<<"Average P in policy 3 is: "<<(double)AVG_P+poffset3/TIME_LIMIT<<" dbm"<<endl;
    return 0;
}
float dist(float x1,float y1,float x2,float y2){
    float d = (float)sqrt(pow(x1-x2,2)+pow(y1-y2,2));
    return d<=2?2.1:d;
}
float getP(float firstmeter,float distance){
    return firstmeter-20*log10(distance-1);
}

int handOffPolicy(vector<car>&ref_car,vector<vector<int> >base,int policy){
    float wold; // quantum of p old
    float curp = -100000;
    int curi; // new index of base
    int handoffnum = 0;
    float x1,x2,y1,y2; // x1 y1 for car's position; x2 y2 for base's position
    double localoff = 0;
    if(policy==1){
        for(int i = 0; i < ref_car.size();++i){
            curi = -1;
            x1 = ref_car[i].x;
            y1 = ref_car[i].y;
            //cout<<ref_car[i].pold[0]<<endl;
            x2 = base[ref_car[i].pold[0]][0];
            y2 = base[ref_car[i].pold[0]][1];
            wold = getP(-60,dist(x1,y1,x2,y2));
            // calculate average index
            localoff += wold - AVG_P;
            for(int j = 0; j < base.size(); ++j){
                if(j==ref_car[i].pold[0])
                    continue;
                x2 = base[j][0];
                y2 = base[j][1];
                if(getP(-60,dist(x1,y1,x2,y2))>wold){
                    wold = getP(-60,dist(x1,y1,x2,y2));
                    curi = j;
                }
            }
            // update handoff number
            if(curi>=0){
                // handoff
                handoffnum++;
                ref_car[i].pold[0] = curi;
            }
        }
        if(ref_car.size())
            poffset1 += localoff/ref_car.size();
    }else if(policy==2)
    {
        float tmp_old;
        for(int i = 0; i < ref_car.size();++i){
            curi = -1;
            x1 = ref_car[i].x;
            y1 = ref_car[i].y;

            x2 = base[ref_car[i].pold[1]][0];
            y2 = base[ref_car[i].pold[1]][1];
            tmp_old = wold = getP(-60,dist(x1,y1,x2,y2));
            // calculate average index
            localoff += wold - AVG_P;
            for(int j = 0; j < base.size(); ++j){
                if(j==ref_car[i].pold[1])
                    continue;
                x2 = base[j][0];
                y2 = base[j][1];
                if(getP(-60,dist(x1,y1,x2,y2))>wold){
                    wold = getP(-60,dist(x1,y1,x2,y2));
                    curi = j;
                }
            }
            // update handoff number
            if(ref_car[i].pold[1]!=curi&&curi>=0&&tmp_old<-110){
                // handoff
                handoffnum++;
                ref_car[i].pold[1] = curi;
            }
        }
        if(ref_car.size())
            poffset2 += localoff/ref_car.size();
    }else if(policy==3){
        float tmp_old_3;
        for(int i = 0; i < ref_car.size();++i){
            curi = -1;
            x1 = ref_car[i].x;
            y1 = ref_car[i].y;

            x2 = base[ref_car[i].pold[2]][0];
            y2 = base[ref_car[i].pold[2]][1];
            tmp_old_3 = wold = getP(-60,dist(x1,y1,x2,y2));
            // calculate average index
            localoff += wold - AVG_P;
            for(int j = 0; j < base.size(); ++j){
                if(j==ref_car[i].pold[2])
                    continue;
                x2 = base[j][0];
                y2 = base[j][1];
                if(getP(-60,dist(x1,y1,x2,y2))>wold){
                    wold = getP(-60,dist(x1,y1,x2,y2));
                    curi = j;
                }
            }
            // update handoff number
            if(ref_car[i].pold[2]!=curi&&curi>=0&&(tmp_old_3+5<wold)){
                // handoff
                handoffnum++;
                ref_car[i].pold[2] = curi;
            }
        }
        if(ref_car.size())
            poffset3 += localoff/ref_car.size();
    }

    return handoffnum;
}
void checkBound(vector<car>&ref_car){
    for(int i = ref_car.size()-1; i >= 0; --i){
        if(ref_car[i].x<1||ref_car[i].x>2999||ref_car[i].y<1||ref_car[i].y>2999){
            cout<<ref_car[i].x<<" "<<ref_car[i].y<<endl;
            ref_car.erase(ref_car.begin()+i);
        }
    }
}
void moveCar(vector<car>&ref_car,vector<vector<int> >corner){

    float p;
    float v = (float)400/36; // m/s
    int direction;
    vector<vector<float> >dir_factor{{0,-1},
                                     {1,0},
                                     {0,1},
                                     {-1,0}};
    for(int i = 0; i < ref_car.size(); ++i){
        p = (float)rand()/RAND_MAX;
        direction = ref_car[i].dir;
        ref_car[i].x += dir_factor[direction][0]*v;
        ref_car[i].y += dir_factor[direction][1]*v;
        // check whether change direction
        for(int j = 0; j < corner.size(); ++j){
            if(abs(ref_car[i].x-(float)corner[j][0])<2 && abs(ref_car[i].y-(float)corner[j][1])<2){

                p = (float)rand()/RAND_MAX;
                //cout<<"change direction "<<p<<endl;
                //cout<<ref_car[i].x<<" "<<ref_car[i].y<<endl;
                if(p<=0.5)
                    ref_car[i].dir = ref_car[i].dir;
                else if(p<=0.83333) // turn right
                    ref_car[i].dir = (ref_car[i].dir+1)%4;
                else // turn left
                    ref_car[i].dir = (ref_car[i].dir+3)%4;

                    /*
                    need to round new x new y
                    */
                ref_car[i].x = (float)corner[j][0];
                ref_car[i].y = (float)corner[j][1];
            }
        }
    }
}
void geneCar(vector<car>&ref_car,vector<vector <int> >entry,float poip){
    // called by each iterate, each call generate at most 12 cars in 12 entries
    float p;
    int pold;
    for(int i = 0; i < entry.size(); ++i){
        p = (float)rand()/RAND_MAX;
        if(i==1||i==3||i==0)
            pold = 0;
        else if(i==2||i==9||i==10)
            pold = 1;
        else if(i==5||i==6||i==4)
            pold = 2;
        else
            pold = 3;
        if(p<=poip){
            // generate car
            car tmpcar{(float)entry[i][0],(float)entry[i][1],{pold,pold,pold,pold},i,entry[i][2]};
            //car tmpcar{(float)entry[10][0],(float)entry[10][1],0,i,3};
            ref_car.push_back(tmpcar);
            n++;
        }
    }

    return;
}
