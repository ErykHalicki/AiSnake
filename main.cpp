#include </opt/homebrew/Cellar/glew/2.2.0_1/include/GL/glew.h> // include GLEW and new version of GL on Windows
#include </opt/homebrew/Cellar/glfw/3.3.8/include/GLFW/glfw3.h> // GLFW helper library for window management
#include <iostream> //for cout
#include <vector>
#include <unordered_map>
#include <sys/time.h>
#include <cstdlib>
#include <array>
#include <fstream>
#include <string>
#include </opt/homebrew/Cellar/sfml/2.5.1_2/include/SFML/audio.hpp>
int SCREEN_WIDTH=900,SCREEN_HEIGHT=900;
using namespace std;
sf::SoundBuffer eatBuffer,endBuffer;
sf::Sound eatSound,endSound;
sf::Music music;

void init(){//misc things i dont understand yet
   	glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
   	glMatrixMode(GL_PROJECTION);
    	//glLoadIdentity();
    	glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1);
    	glMatrixMode(GL_MODELVIEW);
    	//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    	glLoadIdentity();
	eatBuffer.loadFromFile("eat.wav");
	endBuffer.loadFromFile("end.wav");
	eatSound.setBuffer(eatBuffer);
	endSound.setBuffer(endBuffer);
	music.openFromFile("music.wav");
	music.setVolume(15.0f);
	music.setLoop(true);
	music.play();
}
class line{
public:
    float verts[6],oVerts[6];
    float width;
    line(){
    }
    line(float x1, float x2, float y1, float y2, float w){
        verts[0]=x1;
        verts[1]=y1;
        verts[2]=0.0;
        verts[3]=x2;
        verts[4]=y2;
        verts[5]=0.0;
        oVerts[0]=x1;
        oVerts[1]=y1;
        oVerts[2]=0.0;
        oVerts[3]=x2;
        oVerts[4]=y2;
        oVerts[5]=0.0;
        width=w;
    }
    void setRelativePos(float x, float y){
        verts[0]+=x;
        verts[1]+=y;
        verts[3]+=x;
        verts[4]+=y;
    }
    void multiply(float multi){
        verts[0]*=multi;
        verts[1]*=multi;
        verts[3]*=multi;
        verts[4]*=multi;
    }
    void reset(){
        for(int i=0;i<6;i++){
            verts[i]=oVerts[i];
        }
    }
    void toString(){
        cout<< "x1:"<<verts[0]<<" y1:"<<verts[1]<<"\n";
    }
};
void drawFan(vector<pair<float,float>> points,float r, float g, float b){
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    float f[(points.size()+1)*3];
    int i=0;
    for(;i<points.size();i++){
        f[i*3]=points[i].first;
        f[i*3+1]=points[i].second;
        f[i*3+2]=0;
    }
    f[i*3]=points[1].first;
    f[i*3+1]=points[1].second;
    f[i*3+2]=0;
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer(3, GL_FLOAT, 0, f);
    glColor3f(r,g,b);
    glDrawArrays( GL_TRIANGLE_FAN, 0, int(points.size())+1);
    glDisableClientState( GL_VERTEX_ARRAY );
}
void drawRect(int x, int y, int width, int height,float r, float g, float b, float a){
    float verts[12];
    verts[0]=x+width;
    verts[1]=y+height;
    verts[2]=0.0;
    verts[3]=x;
    verts[4]=y+height;
    verts[5]=0.0;
    verts[6]=x;
    verts[7]=y;
    verts[8]=0.0;
    verts[9]=x+width;
    verts[10]=y;
    verts[11]=0.0;
    glColor4f(r,g,b,a);
    glEnableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're using a vertex array for fixed-function attribute
    glVertexPointer( 3, GL_FLOAT, 0, verts ); // point to the vertices to be used
    glDrawArrays( GL_QUADS, 0, 4 ); // draw the vertixes
    glDisableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're finished using the vertex arrayattribute
}
void drawLine(line l){
    glColor3f(0.1,0.1,0.1);
    glEnable( GL_LINE_SMOOTH );
    //glEnable( GL_LINE_STIPPLE );
    //glPushAttrib( GL_LINE_BIT );
    glLineWidth( l.width );
    glLineStipple( 1, 0x00FF );
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, l.verts);
    glDrawArrays( GL_LINES, 0, 2 );
    glDisableClientState( GL_VERTEX_ARRAY );
    //glPopAttrib( );
    //glDisable( GL_LINE_STIPPLE );
    glDisable( GL_LINE_SMOOTH );
}
void drawLine(line l, float intensity){
    glColor3f(intensity,intensity,intensity);
    glEnable( GL_LINE_SMOOTH );
    //glEnable( GL_LINE_STIPPLE );
    //glPushAttrib( GL_LINE_BIT );
    glLineWidth( l.width );
    glLineStipple( 1, 0x00FF );
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, l.verts);
    glDrawArrays( GL_LINES, 0, 2 );
    glDisableClientState( GL_VERTEX_ARRAY );
    //glPopAttrib( );
    //glDisable( GL_LINE_STIPPLE );
    glDisable( GL_LINE_SMOOTH );
}
class snake{
public:
    vector<pair<int,int>> segments;
    pair<int,int> velocity,vel;
    bool alive,player;
    int length;
    vector<pair<int,int>> path;
    snake(){
    }
    snake(pair<int,int> startPos,bool play){
        alive=true;
        player=play;
        length=2;
        velocity=make_pair(0, -1);//1 is up, 1 is left
        segments.push_back(startPos);
        segments.push_back(pairDiff(startPos,velocity));
    }
    pair<int,int> pairDiff(pair<int,int> pair1, pair<int,int> pair2){
        return make_pair(pair1.first-pair2.first, pair1.second-pair2.second);
    }
    void addSegment(){
        segments.insert(segments.end(),pairDiff(segments[segments.size()-1],pairDiff(segments[segments.size()-2],segments[segments.size()-1])));//placing a new segment behind the last 2 segments (2 segments needed to know the velocity direction)
        length++;
    }
    void move(){
        if(vel.first!=velocity.first*-1 && vel.second!=velocity.second*-1){
            velocity=vel;
        }
        for(int i=length-2;i>=0;i--){
            segments[i+1]=make_pair(segments[i].first,segments[i].second);
            //shift all to the previous position of their earlier segment
            //figure out how collision testing, figure out how determine apple position, figure out all logistics
        }
        segments[0]=pairDiff(segments[0], velocity);
    }
    bool tick(pair<int,int> foodPos){
        if(alive){
            if(!player && path.size()>1){
                path.pop_back();
                velocity=pairDiff(path.back(), head());
                velocity.first*=-1;
                velocity.second*=-1;
            }
            move();
            pair<int,int> temp=pairDiff(foodPos, head());
            if(temp.first==0 && temp.second==0){
                addSegment();
                return true;
            }
        }
        return false;
    }
    bool checkBounds(u_long w, u_long h, vector<pair<int,int>> enemy){
        pair<int,int> pos=head();
        if(pos.first<0 || pos.first>w || pos.second<0 || pos.second>w){
            return false;
        }
        for(pair<int,int> p: enemy){
            pair<int,int> pd=pairDiff(head(), p);
            if(pd.first==0 && pd.second==0)return false;
        }
        for(int i=1;i<segments.size();i++){
            pair<int,int> pd=pairDiff(head(), segments[i]);
            if(pd.first==0 && pd.second==0)return false;
        }
        return true;
    }
    pair<int,int> head(){
        return segments[0];
    }
};
class grid{
public:
    vector<line> lines;
    vector<vector<int>> squares;
    bool linesVisible,boxsVisible,startClear,endClear;
    int spacing;
    pair<int, int> startPos, endPos;
    const int imax=2147483647;
    vector<pair<int,int>> path;
    grid(int s){
        linesVisible=true;
        boxsVisible=true;
        startClear=true;
        endClear=true;
        spacing=s;
        for(int i=0;i<SCREEN_WIDTH/spacing;i++){
            lines.insert(lines.end(),line(i*spacing,i*spacing,SCREEN_HEIGHT,0,1));
        }
        for(int i=0;i<SCREEN_HEIGHT/spacing;i++){
            lines.insert(lines.end(),line(0,SCREEN_WIDTH,i*spacing,i*spacing,1));
        }
        for(int i=0;i<SCREEN_HEIGHT/spacing;i++){
            vector<int> temp;
            for(int j=0;j<SCREEN_WIDTH/spacing;j++){
                temp.insert(temp.end(),0);
            }
            squares.insert(squares.end(),temp);
        }
    }
    void clearSquares(){
        for(int i=0;i<squares.size(); i++){
            for(int j=0;j<squares[i].size();j++){
                squares[i][j]=0;
            }
        }
        startClear=true;
        endClear=true;
        path.clear();
    }
    void setBoxs(vector<pair<int,int>> set, int c){
        for(pair<int,int> p:set){
            squares[p.second][p.first]=c;
        }
    }
    void selectBox(pair<int,int> p){
    	squares[p.second][p.first]=3;
    }
    void selectBox(int x, int y,bool start, bool end){
        if(x<SCREEN_WIDTH && y>0 && x>0 && y<SCREEN_HEIGHT){
            if(start){
                startPos=make_pair(x/spacing,y/spacing);
                squares[y/spacing][x/spacing]=1;
                startClear=false;
                return;
            }
            if(end){
                squares[y/spacing][x/spacing]=2;
                endPos=make_pair(x/spacing,y/spacing);
                endClear=false;
                return;
            }
            if(squares[y/spacing][x/spacing]==0){//0=empty, 1=start, 2=end, 3=wall
                squares[y/spacing][x/spacing]=3;
            }
        }
    }
    void draw(){
        if(linesVisible){
            for(line l:lines){
                drawLine(l);
            }
        }
        if(boxsVisible){
            for(int i=0;i<squares.size(); i++){
                for(int j=0;j<squares[i].size();j++){
                    if(squares[i][j]==3){
                        drawRect(j*spacing,i*spacing,spacing,spacing,1,1,1,1);
                    }
                    if(squares[i][j]==6){
                        drawRect(j*spacing,i*spacing,spacing,spacing,0.2,0.2,0.2,1);
                    }
                    if(squares[i][j]==1){
                        drawRect(j*spacing,i*spacing,spacing,spacing,1,0,0,1);
                    }
                    if(squares[i][j]==2){
                        drawRect(j*spacing,i*spacing,spacing,spacing,1,0,0,1);
                    }
                    if(squares[i][j]==4){
                        drawRect(j*spacing,i*spacing,spacing,spacing,0,1,0,1);
                    }
                    if(squares[i][j]==5){
                        drawRect(j*spacing,i*spacing,spacing,spacing,1,0,0,1);
                    }
                }
            }
        }
    }
    pair<int,int> setMin(vector<vector<int>> dist, vector<vector<bool>> searched){
        pair<int,int> result;
        int min=imax;
        for(int i=0;i<squares.size(); i++){
            for(int j=0;j<squares[i].size();j++){
                if(dist[i][j]<min && !searched[i][j]){
                    min=dist[i][j];
                    result=make_pair(j,i);//x,y
                }
            }
        }
        return result;
    }
    vector<pair<int,int>> adjacent(pair<int,int> p){
        vector<pair<int,int>> result;
        if(p.second<squares.size()-1 && squares[p.second+1][p.first]!=3 && squares[p.second+1][p.first]!=4 && squares[p.second+1][p.first]!=5){
            result.insert(result.end(), make_pair(p.first, p.second+1));
        }
        if(p.first<squares[0].size()-1 && squares[p.second][p.first+1]!=3 && squares[p.second][p.first+1]!=4 && squares[p.second][p.first+1]!=5){
            result.insert(result.end(), make_pair(p.first+1, p.second));
        }
        if(p.second>0 && squares[p.second-1][p.first]!=3 && squares[p.second-1][p.first]!=4 && squares[p.second-1][p.first]!=5){
            result.insert(result.end(), make_pair(p.first, p.second-1));
        }
        if(p.first>0 && squares[p.second][p.first-1]!=3 && squares[p.second][p.first-1]!=4 && squares[p.second][p.first-1]!=5){
            result.insert(result.end(), make_pair(p.first-1, p.second));
        }
        return result;
    }
    pair<int,int> adjMin(vector<vector<int>> dist, pair<int,int> p){
        int min=imax;
        pair<int,int> result;
        for(pair<int,int> a:adjacent(p)){
            if(min>dist[a.second][a.first]){
                min=dist[a.second][a.first];
                result=a;
            }
        }
        return result;
    }
    pair<int,int> randomEmpty(){
        bool done=false;
        pair<int,int> result;
        while(!done){
            result=make_pair(rand()%(SCREEN_WIDTH/spacing-2)+1,rand()%(SCREEN_HEIGHT/spacing-2)+1);
            if(squares[result.second][result.first]==0){
                done=true;
            }
        }
        return result;
    }
    vector<pair<int,int>> dijktras(){//returns shortest set of moves from the end to start point or vice verse  (doesnt matter just reverse order)
        vector<pair<int,int>> result;
        vector<vector<int>> dist;
        vector<vector<bool>> searched;
        if(path.size()>0){
            for(pair<int,int> p:path){
                if(squares[p.second][p.first]==6)squares[p.second][p.first]=0;
            }
        }
        for(int i=0;i<squares.size(); i++){
            vector<int> distTemp;
            vector<bool> searchedTemp;
            for(int j=0;j<squares[i].size();j++){
                if(squares[i][j]==1){
                    distTemp.push_back(0);
                }
                else{
                    distTemp.push_back(imax);
                }
                searchedTemp.push_back(false);
            }
            dist.push_back(distTemp);
            searched.push_back(searchedTemp);
        }
        pair<int,int> curr=startPos;
        vector<pair<int,int>> adj;
        bool searching=true;
        int its=0;
        while(its<squares.size()*squares.size()&&searching){
            searched[curr.second][curr.first]=true;
            adj=adjacent(curr);
            for(pair<int,int> p:adj){
                dist[p.second][p.first]=std::min(dist[curr.second][curr.first]+1,dist[p.second][p.first]);
                if(squares[p.second][p.first]==2){
                    searching=false;
                    curr=p;
                    break;
                }
            }
            if(searching){
                curr=setMin(dist,searched);
            }
            its++;
        }
        if(its==squares.size()*squares.size()){
            curr=randomEmpty();
            result.push_back(curr);
            return result;
        }
        pair<int,int> temp;
        result.push_back(curr);
        while(squares[curr.second][curr.first]!=1){
            temp=adjMin(dist,curr);
            result.push_back(temp);
            if(squares[curr.second][curr.first]==0)squares[curr.second][curr.first]=6;
            curr=temp;
        }
        path=result;
        return result;
    }
};

class font{
public:
    int width, size;
    unordered_map<char,vector<line>> data;
    font(){
        
    }
    font(int w, int s){
        width=w;
        size=s;
        getLetters(width);
    }
    void getLetters(int w){
        vector<line> temp;
        unordered_map<char,vector<line>> result;
        string l;
        char curr;
        int n=0;
        float in[4];
        ifstream file ("letters.txt");//why not reading?
        if (file.is_open()){
            while (getline (file,l)){
                if(l[1]==':'){
                    if(n!=0)result.insert(make_pair(curr,temp));
                    temp.clear();
                    n++;
                    curr=l[0];
                    continue;
                }
                for(int i=0;i<4;i++){
                    if(l[i]-'0'==1)in[i]=1;
                    else in[i]=(l[i]-'0')/10.0;
                }
                line l(in[0],in[2],in[1],in[3],w);
                l.multiply(size);
                //l.toString();
                temp.push_back(l);
            }
        result.insert(make_pair(curr,temp));
        file.close();
        }
        else cout << "Unable to open file";
        data=result;
    }
    void drawText(string text,pair<int,int> pos){
        for(int i=0;i<text.length();i++){
            for(line l:data.at(toupper(text[i]))){
                l.setRelativePos(pos.first+i*(size+size/3), pos.second);
                drawLine(l,1);
            }
        }
    }
};
class game{
public:
    grid g=grid(30);
    snake player;
    snake ai;
    pair<int,int> food;
    font f;
    bool aiOn=true;
    int ticks=0;
    game(){
        f=font(2,30);
        ai=snake(make_pair(10,15),false);
        player=snake(make_pair(20,15),true);
        food=g.randomEmpty();
        g.selectBox(food.first*g.spacing,food.second*g.spacing, false, true);
        g.selectBox(ai.head().first*g.spacing, ai.head().second*g.spacing, true, false);
        ai.path=g.dijktras();
    }
    void resetGame(){
        g.clearSquares();
        if(aiOn)ai=snake(make_pair(10,15),false);
        player=snake(make_pair(20,15),true);
        food=g.randomEmpty();
        g.selectBox(food.first*g.spacing,food.second*g.spacing, false, true);
        if(aiOn){
            g.selectBox(ai.head().first*g.spacing, ai.head().second*g.spacing, true, false);
            ai.path=g.dijktras();
        }
    }
    int tick(GLFWwindow *window,long *elapsed){
        /*
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS){
            g.selectBox(xpos,SCREEN_HEIGHT-ypos,false,false);
        }
         */
        if(glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS){
            player.vel=make_pair(0,1);
        }
        if(glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS){
            player.vel=make_pair(1,0);
        }
        if(glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS){
            player.vel=make_pair(0,-1);
        }
        if(glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS){
            player.vel=make_pair(-1,0);
        }
        if(*elapsed>=fmax(100-player.segments.size()-ai.segments.size(),10)){
            ticks++;
            if(player.alive)g.setBoxs(player.segments, 0);
            if(aiOn){
                if(ai.alive)g.setBoxs(ai.segments, 0);
                if(player.tick(food) || ai.tick(food)){
                    if(player.alive)g.setBoxs(player.segments, 4);
                    if(ai.alive)g.setBoxs(ai.segments,5);
                    food=g.randomEmpty();
		    eatSound.play();
                    g.selectBox(food.first*g.spacing, food.second*g.spacing, false, true);
                }
                if(player.checkBounds(g.squares.size()-1, g.squares[0].size()-1,ai.segments) && player.alive){
                    g.setBoxs(player.segments, 4);
                }
                else{
                    player.alive=false;
		    endSound.play();
                    return 1;
                }
                if(ai.checkBounds(g.squares.size()-1, g.squares[0].size()-1,player.segments)&& ai.alive){
                    g.setBoxs(ai.segments, 5);
                    g.selectBox(ai.head().first*g.spacing, ai.head().second*g.spacing, true, false);
                    ai.path=g.dijktras();
                    ticks=0;
                }
                else{
                    ai.alive=false;
                }
            }
            else{
                if(player.tick(food)){
                    if(player.alive)g.setBoxs(player.segments, 4);
                    food=g.randomEmpty();
		    g.selectBox(g.randomEmpty());
                    g.selectBox(food.first*g.spacing, food.second*g.spacing, false, true);
                }
                if(player.checkBounds(g.squares.size()-1, g.squares[0].size()-1,{}) && player.alive){
                    g.setBoxs(player.segments, 4);
                }
                else{
                    player.alive=false;
                    return 1;
                }
            }
            *elapsed=0;
        }
        g.draw();
        if(aiOn){
            f.drawText("ai score:"+std::to_string(ai.segments.size()), make_pair(10, SCREEN_HEIGHT-f.size-10));
            f.drawText("score:"+std::to_string(player.segments.size()), make_pair(10, SCREEN_HEIGHT-f.size*2-20));
        }
        else{
            f.drawText("score:"+std::to_string(player.segments.size()), make_pair(10, SCREEN_HEIGHT-f.size-10));
        }
        return 0;
    }
};
class wonkyFan{
public:
    vector<pair<float,float>> fanPoints,endPoints,startPoints;
    pair<float,float> centre;
    vector<float> speeds,direction;
    pair<float,float> temp;
    int points, rad;
    wonkyFan(){
        
    }
    wonkyFan(int x, int y, int r, int p, int multiMax,int multiMin){
        rad=r;
        points=p;
        centre=make_pair(x, y);
        fanPoints.push_back(centre);
        startPoints.push_back(centre);
        endPoints.push_back(centre);
        speeds.push_back(0);
        direction.push_back(0);
        for(int i=0;i<=points-1;i++){
            startPoints.push_back(make_pair(rad*sin((2*3.14)/points*i)+centre.first, rad*cos((2*3.14)/points*i)+centre.second));
            fanPoints.push_back(make_pair(rad*sin((2*3.14)/points*i)+centre.first, rad*cos((2*3.14)/points*i)+centre.second));
            endPoints.push_back(make_pair(rad*((rand()%multiMax+multiMin)/10.0)*sin((2*3.14)/points*i)+centre.first, rad*((rand()%multiMax+multiMin)/10.0)*cos((2*3.14)/points*i)+centre.second));
            speeds.push_back(rand()%40+30);
            direction.push_back(1);
        }
    }
    void update(int reapeats){
        for(int j=0;j<reapeats;j++){
            for(int i=1;i<fanPoints.size();i++){
                fanPoints[i].first+=(endPoints[i].first-startPoints[i].first)/speeds[i];
                fanPoints[i].second+=(endPoints[i].second-startPoints[i].second)/speeds[i];
                if(abs(fanPoints[i].first-startPoints[i].first)>=abs(endPoints[i].first-startPoints[i].first) && abs(fanPoints[i].second-startPoints[i].second)>=abs(endPoints[i].second-startPoints[i].second)){
                    temp=endPoints[i];
                    endPoints[i]=startPoints[i];
                    startPoints[i]=temp;
                }
            }
        }
    }
};
class guiElement{
public:
    pair<int,int> pos;
    string text;
    bool selectable;
    font f;
    int value;
    guiElement(int x, int y, string t, font fo, bool s, int v){
        pos=make_pair(x,y);
        text=t;
        f=fo;
        selectable=s;
        value=v;
    }
    void draw(){
        f.drawText(text,pos);
    }
};
class screen{
public:
    vector<line> pointerLines;
    font h=font(10,60);;
    font h1=font(4,30);
    int selected=0;
    vector<guiElement> elements;
    screen(){
        pointerLines.push_back(line(0,1,0.5,1,2));
        pointerLines.push_back(line(0,1,0.5,0,2));
    }
    int tick(GLFWwindow *window,long *elapsed, int* lastKey){
        for(guiElement e: elements){
            e.draw();
        }
        if(glfwGetKey(window, GLFW_KEY_S)==GLFW_RELEASE){
            if(*lastKey==GLFW_KEY_S){
                selected++;
                if(selected>=elements.size()){
                    selected=0;
                }
                while(!elements[selected].selectable){
                    selected++;
                    if(selected>=elements.size()){
                        selected=0;
                    }
                }
                *lastKey=0;
            }
        }
        else{
            *lastKey=GLFW_KEY_S;
        }
        if(glfwGetKey(window, GLFW_KEY_W)==GLFW_RELEASE){
            if(*lastKey==GLFW_KEY_W){
                selected--;
                if(selected<0){
                    selected=int(elements.size()-1);
                }
                while(!elements[selected].selectable){
                    selected--;
                    if(selected<0){
                        selected=int(elements.size()-1);
                    }
                }
                *lastKey=0;
            }
        }
        else{
            *lastKey=GLFW_KEY_W;
        }
        if(glfwGetKey(window, GLFW_KEY_ENTER)==GLFW_RELEASE){
            if(*lastKey==GLFW_KEY_ENTER){
                *lastKey=0;
                return elements[selected].value;
            }
        }
        else{
            *lastKey=GLFW_KEY_ENTER;
        }
        if(elements[selected].selectable){
            guiElement e=elements[selected];
            for(line l: pointerLines){
                l.reset();
                l.multiply(e.f.size);
                l.setRelativePos(e.pos.first+e.text.length()*e.f.size+e.f.size*1.5, e.pos.second);
                drawLine(l,1);
            }
        }
        return 0;
    }
};
class titleScreen: public screen{
public:
    wonkyFan one,two;
    titleScreen(){
        one=wonkyFan(150, 400, 50, 15, 30,15);
        two=wonkyFan(775, 400, 50, 15, 30,15);
        string temp="snake";
        elements.push_back(guiElement(SCREEN_WIDTH/2-(temp.length()/2.0+1)*h.size,SCREEN_HEIGHT/4*3,temp,h,false,0));
        elements.back().value=0;
        temp="solo";
        elements.push_back(guiElement(SCREEN_WIDTH/2-(temp.length()/2.0+1)*h1.size,SCREEN_HEIGHT/4*2,temp,h1,true,1));
        temp="ai";
        elements.push_back(guiElement(SCREEN_WIDTH/2-(temp.length()/2.0+1)*h1.size,SCREEN_HEIGHT/4*1.5,temp,h1,true,2));
        temp="exit";
        elements.push_back(guiElement(SCREEN_WIDTH/2-(temp.length()/2.0+1)*h1.size,SCREEN_HEIGHT/4,temp,h1,true,3));
    }
    void secondary(long* elapsed, int* lastKey){
        drawFan(one.fanPoints,0,0.75,0);
        drawFan(two.fanPoints,0.75,0,0);
        if(*elapsed>=10){
            if(*lastKey==0){
                one.update(1);
                two.update(1);
            }
            else{
                one.update(10);
                two.update(10);
            }
            *elapsed=0;
        }
    }
};
class endScreen: public screen{
public:
    wonkyFan one,two;
    endScreen(){
        one=wonkyFan(150, 400, 50, 15, 30,15);
        two=wonkyFan(775, 400, 50, 15, 30,15);
        string temp="GG";
        elements.push_back(guiElement(SCREEN_WIDTH/2-(temp.length()/2.0)*h.size,SCREEN_HEIGHT/4*3,temp,h,false,0));
        elements.back().value=0;
        temp="retry";
        elements.push_back(guiElement(SCREEN_WIDTH/2-(temp.length()/2.0)*h1.size,SCREEN_HEIGHT/4*1.5,temp,h1,true,1));
        temp="exit";
        elements.push_back(guiElement(SCREEN_WIDTH/2-(temp.length()/2.0)*h1.size,SCREEN_HEIGHT/4,temp,h1,true,2));
    }
    void secondary(long* elapsed, int* lastKey){
        drawFan(one.fanPoints,0,0.75,0);
        drawFan(two.fanPoints,0.75,0,0);
        if(*elapsed>=10){
            if(*lastKey==0){
                one.update(1);
                two.update(1);
            }
            else{
                one.update(10);
                two.update(10);
            }
            *elapsed=0;
        }
    }
};
int main(){
    GLFWwindow *window;
    // Initialize the library
    if (!glfwInit())return -1;
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "maze", NULL, NULL);
    if (!window){//if window failed to init
        glfwTerminate( );
        return -1;
    }
    glfwMakeContextCurrent(window);//making "context" current? dont understand
    init();
    glfwSetWindowPos(window, 0, 0);
    glfwFocusWindow(window);
    game g;
    titleScreen t;
    endScreen e;
    double xpos, ypos;
    timeval start,end;
    long elapsed=0;
    int state=0, ret;
    int lastKey=0;
    while (!glfwWindowShouldClose(window)&&state!=-1){//while the window is not closed
        gettimeofday(&start, 0);
        glClear( GL_COLOR_BUFFER_BIT );//clear the screen
        glfwGetCursorPos(window, &xpos, &ypos);
        if(state==0){
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
            ret=t.tick(window,&elapsed,&lastKey);
            t.secondary(&elapsed,&lastKey);
            if(ret==1){
                g.aiOn=false;
                g.resetGame();
                state=1;
            }
            if(ret==2){
                g.aiOn=true;
                g.resetGame();
                state=1;
            }
            if(ret==3)state=-1;
        }
        if(state==1){
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            ret=g.tick(window,&elapsed);
            if(ret==1)state=2;
        }
        if(state==2){
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
            ret=e.tick(window,&elapsed,&lastKey);
            e.secondary(&elapsed, &lastKey);
            if(ret==1){
                state=0;
            }
            if(ret==2){
                state=-1;
            }
        }
        glfwSwapBuffers(window);//swap the buffer being shown and the buffer that was being drawn to
        glfwPollEvents();//check for screen inputs
        gettimeofday(&end, 0);
        elapsed+=(end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;
    }
    glfwTerminate();
    return 0;
}
//make some gameplay edits
//add difficulty select
// make ai snake respawn
//
