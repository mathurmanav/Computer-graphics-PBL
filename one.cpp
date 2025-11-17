#include <graphics.h>
#include <iostream>
#include <vector>
using namespace std;

// Clipping window boundaries
int xmin = 100, ymin = 100, xmax = 400, ymax = 300;

// ---------------- LINE CLIPPING (COHEN – SUTHERLAND) ----------------

const int INSIDE = 0;
const int LEFT = 1;
const int RIGHT = 2;
const int BOTTOM = 4;
const int TOP = 8;

int computeCode(int x, int y) {
    int code = INSIDE;
    if (x < xmin) code |= LEFT;
    else if (x > xmax) code |= RIGHT;
    if (y < ymin) code |= BOTTOM;
    else if (y > ymax) code |= TOP;
    return code;
}

void cohenSutherlandClip(int x1, int y1, int x2, int y2) {
    int code1 = computeCode(x1, y1);
    int code2 = computeCode(x2, y2);
    bool accept = false;

    while (true) {
        if ((code1 == 0) && (code2 == 0)) {
            accept = true;
            break;
        }
        else if (code1 & code2) {
            break;
        }
        else {
            int codeOut;
            int x, y;

            codeOut = code1 ? code1 : code2;

            if (codeOut & TOP) {
                x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1);
                y = ymax;
            } else if (codeOut & BOTTOM) {
                x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1);
                y = ymin;
            } else if (codeOut & RIGHT) {
                y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1);
                x = xmax;
            } else {
                y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1);
                x = xmin;
            }

            if (codeOut == code1) {
                x1 = x; 
                y1 = y;
                code1 = computeCode(x1, y1);
            } else {
                x2 = x; 
                y2 = y;
                code2 = computeCode(x2, y2);
            }
        }
    }

    setcolor(GREEN);
    if (accept) {
        line(x1, y1, x2, y2);
    } else {
        outtextxy(20, 30, "Line is completely outside the window!");
    }
}

// -------------------- AREA (POLYGON) CLIPPING --------------------

bool inside(int x, int y, int edge) {
    if (edge == 0) return x >= xmin;
    if (edge == 1) return x <= xmax;
    if (edge == 2) return y >= ymin;
    return y <= ymax;
}

pair < int,int > intersect(pair < int,int > p1, pair < int,int > p2, int edge) {
    float x1 = p1.first, y1 = p1.second;
    float x2 = p2.first, y2 = p2.second;
    float x, y;

    if (edge == 0) {
        x = xmin;
        y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1);
    }
    else if (edge == 1) {
        x = xmax;
        y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1);
    }
    else if (edge == 2) {
        y = ymin;
        x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1);
    }
    else {
        y = ymax;
        x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1);
    }

    return {x, y};
}

vector < pair < int,int > > clipPolygon(vector < pair < int,int > > poly) {
    vector < pair < int,int > > output = poly;

    for (int edge = 0; edge < 4; edge++) {
        vector < pair < int,int > >  input = output;
        output.clear();

        for (int i = 0; i < input.size(); i++) {
            pair<int,int> P = input[i];              // current
            pair<int,int> S = input[(i+1) % input.size()]; // next

            bool P_inside = inside(P.first, P.second, edge);
            bool S_inside = inside(S.first, S.second, edge);

            if (P_inside && S_inside) {
                output.push_back(S);
            }
            else if (P_inside && !S_inside) {
                output.push_back(intersect(P, S, edge));
            }
            else if (!P_inside && S_inside) {
                output.push_back(intersect(P, S, edge));
                output.push_back(S);
            }
        }
    }
    return output;
}

// ------------------------ MAIN PROGRAM ------------------------

int main() {
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    rectangle(xmin, ymin, xmax, ymax);
    outtextxy(50, 50, "Line & Polygon Clipping Project");

    int choice;
    cout << "1. Line Clipping\n2. Polygon Clipping\nEnter choice: ";
    cin >> choice;

    if (choice == 1) {
        int x1, y1, x2, y2;
        cout << "Enter line coordinates (x1 y1 x2 y2): ";
        cin >> x1 >> y1 >> x2 >> y2;

        setcolor(RED);
        line(x1, y1, x2, y2);

        cohenSutherlandClip(x1, y1, x2, y2);
    }

    else if (choice == 2) {
        int n;
        cout << "Enter number of vertices: ";
        cin >> n;

        vector < pair < int,int > > poly(n);

        for (int i = 0; i < n; i++) {
            cin >> poly[i].first >> poly[i].second;
        }

        setcolor(RED);
        for (int i = 0; i < n; i++) {
            line(poly[i].first, poly[i].second, 
                 poly[(i + 1) % n].first, poly[(i + 1) % n].second);
        }

        vector < pair < int,int > > clipped = clipPolygon(poly);

        setcolor(GREEN);
        for (int i = 0; i < clipped.size(); i++) {
            line(clipped[i].first, clipped[i].second,
                 clipped[(i + 1) % clipped.size()].first, 
                 clipped[(i + 1) % clipped.size()].second);
        }
    }

    getch();
    closegraph();
    return 0;
}

