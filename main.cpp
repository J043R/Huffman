#include <bits/stdc++.h>
using namespace std;

struct Forest{
    int weight;
    int root;
};

struct Tree{
    int left = -1, right = -1, parent = -1;
};

FILE *f;
FILE *vihod;

Forest forest[256];
Tree tree[512];


//нахождение 2х минимумов
pair <int, int> getmin(int sizef) {
    int m1, m2;
    if (forest[0].weight <= forest[1].weight) {
        m1 = 0;
        m2 = 1;
    } else {
        m1 = 1;
        m2 = 0;
    }
    for(int i = 2; i < sizef; i++) {
        if (forest[i].weight < forest[m1].weight){
            m2 = m1;
            m1 = i;
        } else if (forest[i].weight < forest[m2].weight) {
            m2 = i;
        }
    }
    return{m1, m2};
}

int32_t main(int argc, char *argv[])
{
    if (!strcmp("encode", argv[1]))
    {
        f = fopen(argv[3], "rb");
        vihod = fopen(argv[2], "wb");


        int freq[256];
        unsigned char ch;
        for (int i = 0; i <= 256; i++)
        {
            freq[i] = 0;
        }

        //заполнение частотного словаря
        int symcnt = 0;
        while (fscanf(f, "%c", &ch) != -1)
        {
            freq[ch]++;
            symcnt++;
        }

        int sizef = 0;

        //заполнение foresta
        for (int i = 0; i < 256; i++)
        {
            if (freq[i] != 0)
            {
                forest[sizef].root = sizef;
                forest[sizef].weight = freq[i];
                sizef++;
            }
        }

        //сборка tree
        int sizet = sizef;
        while (sizef > 1) {
            pair <int, int> m = getmin(sizef);

            tree[sizet].left = forest[m.first].root;
            tree[sizet].right = forest[m.second].root;
            tree[sizet].parent = -1;
            tree[forest[m.first].root].parent = sizet;
            tree[forest[m.second].root].parent = sizet;

            forest[m.first].weight = forest[m.first].weight + forest[m.second].weight;
            forest[m.first].root = sizet;
            forest[m.second] = forest[sizef - 1];

            sizef--;
            sizet++;
        }
        //сборка кодов
        map <char, string> codes;
        int ind = 0;
        for (int i = 0; i < 256; i++)
        {
            if (freq[i] > 0)
        {
                string code = "";
                int curind = ind;
                while (tree[curind].parent != -1) {
                    int last = curind;
                    curind = tree[curind].parent;
                    if (tree[curind].left == last) {
                        code += "0";
                    } else {
                        code += "1";
                    }
                }
                reverse(code.begin(), code.end());
                codes[i] = code;
                ind++;
            }
        }

        fprintf(vihod, "%i ", codes.size());
        fprintf(vihod, "%i", symcnt);

        if (codes.size() == 1)
        {
            codes.begin()->second = "1";
        }

        //заполнение шапки
        for (auto [ch2, kod] : codes)
        {
            fprintf(vihod, " %c", ch2);
            fprintf(vihod, " %i ", kod.length());
            fprintf(vihod, "%s", kod.c_str());
        }
        fprintf(vihod, "%c", ' ');

        fclose(f);

        f = fopen(argv[3], "rb");

        unsigned char byte = 0;
        int cntbits = 0;

        //кодировка
        while (fscanf(f, "%c", &ch) != -1)
        {
            string kod = codes[ch];
            for (auto c : kod)
            {
                byte <<= 1;
                byte += c - '0';
                cntbits++;

                if (cntbits == 8)
                {
                    fprintf(vihod, "%c", byte);
                    cntbits = 0;
                    byte = 0;
                }
            }
        }
        if (cntbits > 0)
        {
            byte <<= 8 - cntbits;
            fprintf(vihod, "%c", byte);
            cntbits = 0;
            byte = 0;
        }

        fclose(f);
        fclose(vihod);
    }
    else
    {
        //раскодировка
        unsigned char ch;
        f = fopen(argv[2], "rb");
        vihod = fopen(argv[3], "wb");

        int cntchars, cntsym;
        fscanf(f, "%i", &cntchars);
        fscanf(f, "%i", &cntsym);

        //шапка
        map <int, map<int, char>> uncodes;
        int kodnow = 0;
        int indkodbit = 30;

        for (int i = 0; i < cntchars; i++)
        {
            int len;
            fscanf(f, "%c", &ch);
            fscanf(f, "%c", &ch);
            fscanf(f, "%i", &len);
            char* s = new char[len];
            fscanf(f, "%s", s);
            string trues = string(s);
            for (auto c : trues)
            {
                kodnow += ((int)(c - '0') << indkodbit);
                indkodbit--;
            }
            uncodes[trues.length()][kodnow] = ch;
            kodnow = 0;
            indkodbit = 30;
        }


        fscanf(f, "%c", &ch);
        indkodbit = 30;
        kodnow = 0;
        int indsym = 0;

        //считывание кодов и преобразование в символы
        while (1)
        {
            fscanf(f, "%c", &ch);
            for (int j = 7; j >= 0; j--)
            {
                int bit = ((ch >> j) & 1);
                kodnow += ((int)bit << indkodbit);
                indkodbit--;

                if (uncodes[30 - indkodbit].find(kodnow) != uncodes[30 - indkodbit].end())
                {
                    fprintf(vihod, "%c", uncodes[30 - indkodbit][kodnow]);
                    kodnow = 0;
                    indkodbit = 30;
                    indsym++;
                    if (indsym == cntsym)
                    {
                        break;
                    }
                }
            }
            if (indsym == cntsym)
            {
                break;
            }
        }
    }
    return 0;
}
