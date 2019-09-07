#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

void swap(int &a, int &b)
{
    a = a+b;
    b = a-b;
    a = a-b;
    return;
}

void arrange(vector <int> &numbers)
{
    int n = numbers.size();
    int blips;
    do
    {
        blips = 0;
        for (int i=0; i<n-1; i++)
        {
            if (numbers[i] > numbers[i+1])
            {
                swap(numbers[i], numbers[i+1]);
                blips++;
            }
        }
    }
    while (blips != 0);
    return;
}

bool checkLine(int a, int b, int c)
{
    vector <int> numbers = {a, b, c};
    arrange(numbers);
    
    bool p, q, r;
    p = numbers[0] + numbers[2] == 2*numbers[1];
    q = numbers[1] - numbers[0] == 3;
    r = numbers[1]%3 == 1;
    return (p && (q||r));
}

class Move //yet to write copy constructor and overload of assignment operator. by some magic, it's already there. dafuq!?
{
public:
    bool quantum;
    int alpha, beta, collapsed;
    
    Move() {}
    
    Move(int a, int b)
    {
        quantum = true;
        alpha = a;
        beta = b;
    }
    
    Move (int c)
    {
        quantum = false;
        collapsed = c;
    }
    
};

bool cancelOut(Move &m, int b)
{
    if (m.quantum == false)
    {
        return false;
    }
    
    if (b == m.alpha)
    {
        m.collapsed = m.beta;
        m.quantum = false;
        return true;
    }
    
    if (b == m.beta)
    {
        m.collapsed = m.alpha;
        m.quantum = false;
        return true;
    }
    
    return false;
}

class Game
{
public:
    vector <Move> game;
    bool Occ[9];
    
    Game()
    {
        for (int i=0; i<9; i++)
        {
            Occ[i] = false;
        }
    }
    
    Game(Game &g)
    {
        int n = g.game.size();
        for (int i=0; i<n; i++)
        {
            game.push_back(g.game[i]);
        }
        for (int i=0; i<9; i++)
        {
            Occ[i] = g.Occ[i];
        }
    }
    
    class Lookout
    {
    public:
        int alert, count;
        Lookout() {}
        Lookout(int a, int c)
        {
            alert = a; count = c;
        }
    };
    
    class Watchlist
    {
    public:
        vector <Lookout> watchlist;
        Watchlist() {}
        Watchlist(Game &g, int n)
        {
            switch(n)
            {
                case 0: {watchlist.push_back(Lookout(g.game.at(g.game.size()-1).alpha, g.game.size()-1)); break;}
                case 1: {watchlist.push_back(Lookout(g.game.at(g.game.size()-1).beta, g.game.size()-1)); break;}
                default: break;
            }
        }
        

        
        bool find(int n)
        {
            bool answer = false;
            for (int i=0; i<watchlist.size() && !answer; i++)
            {
                answer = (watchlist.at(i).alert == n && watchlist.at(i).count != 0);
            }
            return answer;
        }
        
        void update()
        {
            for (int i=0; i<watchlist.size(); i++)
            {
                if (watchlist.at(i).count != 0)
                {
                    watchlist.at(i).count--;
                }
            }
        }
        
        bool cancel(Game &g, int index)
        {
            bool answer = false;
            for (int i=0; i<watchlist.size() && !answer; i++)
            {
                if (watchlist.at(i).count == 0) continue;
                answer = cancelOut(g.game.at(index), watchlist.at(i).alert);
            }
            update();
            if (answer)
            {
                watchlist.push_back(Lookout(g.game.at(index).collapsed, g.game.size() -1));
                g.Occ[g.game.at(index).collapsed] = true;
            }
            return answer;
        }
    };
    
    bool collapsible()
    {
        Game analysis(*this);
        Watchlist w(analysis, 0);
        int crosses;
        do
        {
            crosses = 0;
            for (int iter=0; iter < analysis.game.size(); iter++)
            {
                if (w.cancel(analysis, iter))
                {
                    crosses++;
                }
            }
        }
        while (crosses != 0);
        return !analysis.game.at(analysis.game.size()-1).quantum ;
    }
    
    void makeCollapse(int n)
    {
        Watchlist w(*this, n);
        int crosses;
        do
        {
            crosses = 0;
            for (int iter=0; iter < this->game.size(); iter++)
            {
                if (w.cancel(*this, iter))
                {
                    crosses++;
                }
            }
        }
        while (crosses != 0);
    }
    
    int evaluation()
    {
        int size = game.size();
        int evenLines, oddLines;
        vector <int> evenHighests;
        vector <int> oddHighests;
        
        evenLines = 0;
        for (int i=0; i<size; i=i+2)
        {
            if (game.at(i).quantum) continue;
            for (int j = i+2; j<size; j=j+2)
            {
                if (game.at(j).quantum) continue;
                for (int k = j+2; k<size; k=k+2)
                {
                    if (game.at(k).quantum) continue;
                    
                    if (checkLine(game.at(i).collapsed, game.at(j).collapsed, game.at(k).collapsed))
                    {
                        evenLines++;
                        evenHighests.push_back(k);
                        
                    }
                }
            }
        }
        
        oddLines = 0;
        for (int i=1; i<size; i=i+2)
        {
            if (game.at(i).quantum) continue;
            for (int j = i+2; j<size; j=j+2)
            {
                if (game.at(j).quantum) continue;
                for (int k = j+2; k<size; k=k+2)
                {
                    if (game.at(k).quantum) continue;
                    if (checkLine(game.at(i).collapsed, game.at(j).collapsed, game.at(k).collapsed))
                    {
                        oddLines++;
                        oddHighests.push_back(k);
                    }
                }
            }
        }
        if (evenLines == 2)
        {
            if (evenHighests[0] == evenHighests[1]) return 4;
            else return 3;
        }
        if (evenLines == 1)
        {
            if (oddLines == 0) return 2;
            if (oddLines == 1)
            {
                if (evenHighests[0] < oddHighests[0]) return 1;
                else return -1;
            }
        }
        if (evenLines == 0)
        {
            if (oddLines == 0) return 0;
            if (oddLines == 1) return -2;
        }
        return 0;
    }
    
    bool gameOver()
    {
        if (evaluation() != 0) return true;
        if (game.size() != 9) return false;
        bool Fullhouse = true;
        for (int i=0; i<9 && Fullhouse; i++)
        {
            Fullhouse = Occ[i];
        }
        return Fullhouse;
    }
    
    bool validMove(int option, int a, int b) //I guess this is primarily intended for humans.
    {
        if (collapsible())
        {
            makeCollapse(option); //game can be over here, either result possible.
            if (gameOver())
            {
                return true;
            }
        }
        vector <int> unoccupied; int size=0;
        for (int i=0; i<9; i++)
        {
            if (!Occ[i]) {unoccupied.push_back(i); size++;}
        }
        if (size == 1)
        {
            Move m;
            m.quantum = false; m.collapsed = unoccupied[0];
            game.push_back(m);
            Occ[unoccupied[0]] = true;
            return true;
        } //game can be over here.
        if (a == b) return false;
        if (a>8 || b>8 || a<0 || b<0) return false;
        if (Occ[a] || Occ[b]) return false;
        game.push_back(Move(a, b)); //here's where the recursive call is definitely required
        return true;
    }
    
    struct Description
    {
        bool throughCollapse;
        int colChoice; //anything other than 0 or 1 if throughCollapse is false?!
        bool collapseFinish;
        bool forced;
        int onlySquare;
        int qa, qb; //the constructor is called at this point in the story
        
        Description() {}
        
        Description(int code, int a, int b)
        {
            throughCollapse = true;
            colChoice = code;
            collapseFinish = false;
            forced = false;
            qa = a; qb = b;
        }
        
        Description(int a, int b)
        {
            throughCollapse = false;
            collapseFinish = false;
            forced = false;
            qa = a; qb = b;
        }
    };
    
    void play(Description recipe) //use this to play AI move
    {
        if (recipe.throughCollapse)
        {
            makeCollapse(recipe.colChoice);
        
            if (recipe.collapseFinish) return;
            
            if (recipe.forced)
            {
                Move m;
                m.quantum = false; m.collapsed = recipe.onlySquare;
                game.push_back(m);
                Occ[m.collapsed] = true;
                return;
            }
        }
        game.push_back(Move(recipe.qa, recipe.qb));
    }
    
    struct Analysis //because in the algorithm, to make the move you need the recipe, but the recursive call needs to return the evaluation.
    {
        int eval; Description recipe;
        Analysis(int n, Description d)
        {
            eval = n; recipe = d;
        }
    };
    
    Analysis strategy()
    {
        vector <Analysis> possibilities;
        if (game.size()!=0 && collapsible())
        {
            for (int choice = 0; choice <=1; choice++)
            {
                Game roughwork(*this);
                roughwork.makeCollapse(choice);
                if (roughwork.gameOver())
                {
                    int n = roughwork.evaluation();
                    Description d;
                    d.throughCollapse = true;
                    d.colChoice = choice;
                    d.collapseFinish = true;
                    possibilities.push_back(Analysis(n, d));
                }
                else //collapse of choice made on roughwork, game not over...
                {
                    vector <int> unoccupied; int size = 0;
                    for (int i=0; i<9; i++)
                    {
                        if (!roughwork.Occ[i])
                        {
                            unoccupied.push_back(i); size++;
                        }
                    }
                    if (size == 1)
                    {
                        Move m; m.quantum = false; m.collapsed = unoccupied[0];
                        roughwork.game.push_back(m); roughwork.Occ[unoccupied[0]] = true;
                        int n = roughwork.evaluation(); //there are no moves after this...
                        Description d;
                        d.throughCollapse = true; d.colChoice = choice; d.collapseFinish = false; d.forced = true;
                        d.onlySquare = unoccupied[0];
                        possibilities.push_back(Analysis(n, d));
                    }
                    else //collapse of choice made on roughwork, game not over, quantum move(s) can be made.
                    {
                        for (int i = 0; i < size; i++)
                        {
                            for (int j=i+1; j<size; j++)
                            {
                                Description d(choice, unoccupied[i], unoccupied[j]);
                                Game roughwork2(*this);
                                roughwork2.play(d);
                                int n = roughwork2.strategy().eval;
                                possibilities.push_back(Analysis(n, d));
                            }
                        }
                    }
                }
            }
        }
        else //not collapsible, quantum moves available.
        {
            Game roughwork(*this);
            vector <int> unoccupied; int size = 0;
            for (int i=0; i<9; i++)
            {
                if (!roughwork.Occ[i])
                {
                    unoccupied.push_back(i); size++;
                }
            }
            for (int i=0; i<size; i++)
            {
                for (int j=i+1; j<size; j++)
                {
                    Description d(unoccupied[i], unoccupied[j]);
                    Game roughwork2(roughwork);
                    roughwork2.play(d);
                    int n = roughwork2.strategy().eval;
                    possibilities.push_back(Analysis(n, d));
                    
                }
            }
        }
        int factor = pow(-1, game.size());
        int champ = possibilities[0].eval*factor;
        int winner = 0;
        for (int i=1; i<possibilities.size(); i++)
        {
            int challenger = possibilities[i].eval*factor;
            if (challenger > champ) {champ = challenger; winner = i;}
        }
        return possibilities[winner];
    }
    
    void printOcc()
    {
        for (int i=0; i<game.size(); i=i+2)
        {
            if (!game.at(i).quantum) {cout<<game.at(i).collapsed<<" ";}
        }
        cout<<endl;
        for (int i=1; i<game.size(); i=i+2)
        {
            if (!game.at(i).quantum) {cout<<game.at(i).collapsed<<" ";}
        }
        cout<<endl;
    }
};



int main()
{
   
    Game g;
    g.game.push_back(Move(0, 8));
    g.game.push_back(Move(4));
    g.game.push_back(Move(6));
    g.game.push_back(Move(3));
    g.game.push_back(Move(1));
    g.game.push_back(Move(7));
    //g.game.push_back(Move(2, 5)); (7)
    //g.game.push_back(Move(2, 5)); (8)
    g.Occ[1] = true; g.Occ[3] = true; g.Occ[4] = true; g.Occ[6] = true; g.Occ[7] = true;
    
    //g.game.push_back(Move(5, 8));
    Game::Analysis c = g.strategy();
    g.play(c.recipe);
    cout<<c.recipe.qa<<c.recipe.qb<<endl;
    Game::Analysis b = g.strategy();
    g.play(b.recipe);
    cout<<b.recipe.qa<<b.recipe.qb<<endl;
    //cout<<g.strategy().eval<<endl;
    Game::Analysis a = g.strategy();
    g.play(a.recipe);
    
    cout<<a.recipe.qa<<a.recipe.qb<<endl;
    g.play(g.strategy().recipe);
    g.printOcc();
    cout<<g.evaluation()<<endl;
}
