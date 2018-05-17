#include <cmath>
#include <cstring>
#include <iostream>
#include <algorithm>
using namespace std;
#define MAXN 200009

typedef long long int ll;

/*
 * ALGORITMO DE MO
 */
int arr[MAXN], N, sn, Q;
bool appeared[MAXN];
int inv[MAXN];	//mo tree

struct query {
	int l, r, id, lca;
	ll ans;
	query() { l=r=id=lca=-1; }
	query(int _id, int _l, int _r) {
		id = _id; l = _l; r = _r; lca = -1;
	}
} qrs[MAXN];

bool lrcomp(query a, query b) {
	if (a.l/sn != b.l/sn) return a.l/sn < b.l/sn;
  	return a.r > b.r;
}

bool idcomp(query a, query b) {
	return a.id < b.id;
}

ll freq[MAXN], curAns;
void check(int i) {
	if (i < 0 || i >= N) return;
	//if (appeared[i]) {	//mo array
	if (appeared[inv[i]]) {	//mo tree
		freq[arr[i]]--;
		if (freq[arr[i]] == 0) curAns--;
	}
	else {
		if (freq[arr[i]] == 0) curAns++;
		freq[arr[i]]++;
	}
	appeared[inv[i]] = !appeared[inv[i]];	//mo tree
	//appeared[i] = !appeared[i];			//mo array
}

void mo() {
	sn = sqrt(N);
	sort(qrs, qrs+Q, &lrcomp);
	memset(&freq, 0, sizeof freq);
	memset(&appeared, false, sizeof appeared);
	int l = 1, r = 0;
	curAns = 0;
	for(int i=0; i<Q; i++) {
		query & q = qrs[i];
		while(r > q.r) check(r--);
		while(r < q.r) check(++r);
		while(l < q.l) check(l++);
		while(l > q.l) check(--l);
		check(q.lca);
		q.ans = curAns;
		check(q.lca);
	}
	sort(qrs, qrs+Q, &idcomp);
}

/*
 * LCA
 */
#include <vector>
#define MAXLOGN 20
 
typedef pair<int, int> ii;
vector<ii> adjList[MAXN];
int depth[MAXN], level[MAXN];
int P[MAXN][MAXLOGN];

void depthdfs(int u) {
	for(int i=0; i<(int)adjList[u].size(); i++) {
		int v = adjList[u][i].first;
		int w = adjList[u][i].second;
		if (v == P[u][0]) continue;
		P[v][0] = u;
		level[v] = 1 + level[u];
		depth[v] = w + depth[u];
		depthdfs(v);
	}
}
void computeP(int root) {
	level[root] = depth[root] = 0;
	P[root][0] = root;
	depthdfs(root);
	for(int j = 1; j < MAXLOGN; j++)
		for(int i = 1; i <= N; i++)
			P[i][j] = P[P[i][j-1]][j-1];
}
int LCA(int a, int b) {
	if(level[a] > level[b]) swap(a, b);
	int d = level[b] - level[a];
	for(int i=0; i<MAXLOGN; i++) {
		if((d & (1<<i)) != 0) b = P[b][i];
	}
	if(a == b) return a;
	for(int i = MAXLOGN-1; i>=0; i--)
		while(P[a][i] != P[b][i]) {
			a=P[a][i]; b=P[b][i];
		}
	return P[a][0];
}

/*
 * TREE MO
 */

int st[MAXN], en[MAXN], cnt, c[MAXN];
void prepos(int u, int p) {
	arr[cnt] = c[u];
	inv[cnt] = u;
	st[u] = cnt++;
	for(int i=0; i<(int)adjList[u].size(); i++) {
		int v = adjList[u][i].first;
		if (v != p) prepos(v, u);
	}
	arr[cnt] = c[u];
	inv[cnt] = u;
	en[u] = cnt++;
}

void treemo(int root) {
	cnt = 0;
	prepos(root, -1);
	computeP(root);
	N = cnt;
	for(int i=0, u, v, lca; i<Q; i++) {
		query & q = qrs[i];
		u = q.l; v = q.r; lca = LCA(u, v);
		if (st[u] > st[v]) swap(u, v);
		//if (lca == u) q.l = st[u]+1, q.lca = -1;	//propriedade na aresta
		//else q.l = en[u], q.lca = -1;
		if (lca == u) q.l = st[u], q.lca = -1;	//propriedade no noh
		else q.l = en[u], q.lca = st[lca];
		q.r = st[v];
	}
	mo();
}

/*
 * SPOJ COT2
 */
#include <cstdio>
#include <map>
 
int main()
{
	map<int, int> x2id;
	int k=0;
	scanf("%d %d", &N, &Q);
	for(int i=1; i<=N; i++) {
		scanf("%d", c+i);
		if (!x2id.count(c[i])) x2id[c[i]] = k++;
		c[i] = x2id[c[i]];
	}
	int u, v;
	for(int i=1; i<N; i++) {
		scanf("%d %d", &u, &v);
		adjList[u].push_back(ii(v, -1));
		adjList[v].push_back(ii(u, -1));
	}
	int l, r;
	for(int i=0; i<Q; i++) {
		scanf("%d %d", &l, &r);
		qrs[i] = query(i, l, r);		
	}
	treemo(1);
	for(int i=0; i<Q; i++) {
		printf("%lld\n", qrs[i].ans);
	}
	return 0;
}