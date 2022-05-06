#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

const int WINDOW_SIZE_X = 800, WINDOW_SIZE_Y = 800;
const int CELLS_X = 100, CELLS_Y = 100;
const int DRAW_SIZE_X = (WINDOW_SIZE_X / 1.3) / CELLS_X;
const int DRAW_SIZE_Y = (WINDOW_SIZE_Y / 1.3) / CELLS_Y;

                         //l, r, u, d
struct cell_neighbours { int ind[4]; };

struct cell_neighbours get_neighbours(int index)
{
    cell_neighbours out;
    
    out.ind[0] = (!(index % CELLS_X)) ? -1 : index - 1;
    out.ind[1] = (!((index + 1) % CELLS_X)) ? -1 : index + 1;
    out.ind[2] = (index - CELLS_X < 0) ? -1 : index - CELLS_X; 
    out.ind[3] = (index + CELLS_X >= CELLS_X * CELLS_Y) ? -1 : index + CELLS_X;
            
    return out;
}

vector<cell_neighbours> generate_graph()
{
    vector<cell_neighbours> out;
    for(int curr_cell_index = 0;curr_cell_index < CELLS_X * CELLS_Y;curr_cell_index++)
    {
        cell_neighbours curr_cell_neighbours = get_neighbours(curr_cell_index);   
        out.push_back(curr_cell_neighbours);
    
    }
    return out;
}

void randomize(vector<pair<int, int>> &p)
{
    for(int i = 0;i < 10;i++)
    {
        int f_i = rand() % p.size();
        int s_i = rand() % p.size();
        
        auto tmp = p[f_i];
        p[f_i] = p[s_i];
        p[s_i] = tmp;
    }
}


void dfs(int curr, bool *vis,const vector<cell_neighbours> &p, cell_neighbours *out)
{
    vis[curr] = true;
    
    vector<pair<int, int>> options;    
    
    for(int i = 0;i < 4;i++)
    {
        int next = p[curr].ind[i];
        options.push_back({i, next});
    }   

    randomize(options);

    for(auto next : options)
    {       
        if(next.second != -1 && !vis[next.second])
        {
            int index;
            if(next.first == 0) index = 1;
            if(next.first == 1) index = 0;
            if(next.first == 2) index = 3;
            if(next.first == 3) index = 2;
        
            out[curr].ind[next.first] = next.second;
            out[next.second].ind[index] = curr;     
            
            dfs(next.second, vis, p, out); 
        }
    }


    return;
}

cell_neighbours *vis_graph(const vector<cell_neighbours> &p)
{
    bool vis[CELLS_Y * CELLS_Y];
    for(int i = 0;i < CELLS_X * CELLS_Y;vis[i++] = 0);

    cell_neighbours *out =  new cell_neighbours[CELLS_X * CELLS_Y];

    for(int i = 0;i < CELLS_X * CELLS_Y;i++)
        for(int j = 0;j < 4;j++)
            out[i].ind[j] = -1;

    dfs(0, vis, p, out); 

    return out;
}


vector<sf::RectangleShape> draw_maze(cell_neighbours * maze)
{
    int index = 0, x1 = 0, y1 = 0, y2 = DRAW_SIZE_Y, x2 = DRAW_SIZE_X;
    int t = 0;
    
    vector< sf::RectangleShape > out;

    for(int i = 0;i < CELLS_X * CELLS_Y;i++)
    {
        cell_neighbours curr_n = maze[index];
            
        if(!(t % CELLS_X))
        { 
            x1 = 0;
            x2 = DRAW_SIZE_X;
            y1 += DRAW_SIZE_Y;
            y2 += DRAW_SIZE_Y;
        }

        for(int j = 0;j < 4;j++)
        {
            if(curr_n.ind[j] == -1)
            {
                if (j == 0)
                {
                    sf::RectangleShape curr;
                    curr.setSize(sf::Vector2f(1, DRAW_SIZE_Y));
                    curr.setPosition(x1, y1);
                    out.push_back(curr);
                }
                
                if(j == 1)
                {
                    sf::RectangleShape curr;
                    curr.setSize(sf::Vector2f(1, DRAW_SIZE_Y));
                    curr.setPosition(x2, y1);
                    out.push_back(curr);
                }
                
                if(j == 2)
                {
                    sf::RectangleShape curr;
                    curr.setSize(sf::Vector2f(DRAW_SIZE_X, 1));
                    curr.setPosition(x1, y1);
                    out.push_back(curr);
                }
                
                if(j == 3)
                {
                    sf::RectangleShape curr;
                    curr.setSize(sf::Vector2f(DRAW_SIZE_X, 1));
                    curr.setPosition(x1, y2);
                    out.push_back(curr);
                }
            }
        }
        x1 += DRAW_SIZE_X;
        x2 += DRAW_SIZE_X;
        
        t++;
        index++;
    }


    return out;
}

int main()
{
    srand(static_cast<unsigned int> (time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE_Y, WINDOW_SIZE_X), "SFML works!");
    
    vector<cell_neighbours> g = generate_graph();
    
    cell_neighbours *maze = vis_graph(g);
    
    vector<sf::RectangleShape> lines = draw_maze(maze);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
         
       
        window.clear();
        for(int i = 0;i < lines.size();i++)
            window.draw(lines[i]);
        window.display();
    }

    return 0;
}
