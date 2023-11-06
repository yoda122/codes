#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;
ll z = 0, mod = 1000000007, INF = 9999999999999999;

bool tenpai_ment(ll pos, ll shuntu, ll kotu, ll toitu, vector<ll> hai){
  if (shuntu + kotu == 4 && toitu == 1){
    return true;
  }

  while (hai[pos] == 0) pos++;
  if (pos == 38){
    return false;
  }

  bool res = false;
  if (pos < 30 && hai[pos] >= 1 && hai[pos + 1] >= 1 && hai[pos + 2] >= 1){
    hai[pos]--;
    hai[pos + 1]--;
    hai[pos + 2]--;
    ll npos = pos;
    while (hai[npos] == 0) npos++;

    res = res | tenpai_ment(npos, shuntu + 1, kotu, toitu, hai);
    hai[pos]++;
    hai[pos + 1]++;
    hai[pos + 2]++;
  }

  if (hai[pos] >= 2){
    hai[pos] -= 2;
    ll npos = pos;
    while (hai[npos] == 0) npos++;

    res = res | tenpai_ment(npos, shuntu, kotu, toitu + 1, hai);
    hai[pos] += 2;
  }

  if (hai[pos] >= 3){
    hai[pos] -= 3;
    ll npos = pos;
    while (hai[npos] == 0) npos++;

    res = res | tenpai_ment(npos, shuntu, kotu + 1, toitu, hai);
    hai[pos] += 3;
  }

  return res;
}

bool tenpai_chitoi(ll pos, ll toitu, vector<ll> hai){
  if (toitu == 7){
    return true;
  }

  while (hai[pos] == 0) pos++;
  if (pos == 38){
    return false;
  }

  bool res = false;
  if (hai[pos] == 2){
    hai[pos] -= 2;
    ll npos = pos;
    while (hai[npos] == 0) npos++;

    res = res | tenpai_chitoi(npos, toitu + 1, hai);
    hai[pos] += 2;
  }

  return res;
}

bool tenpai_kokusi(vector<ll> hai){
  vector<ll> kokusi = {1, 9, 11, 19, 21, 29, 31, 32, 33, 34, 35, 36, 37};
  bool pai = false;
  for (ll i : kokusi){
    if (hai[i] < 1) return false;
    if (hai[i] == 2) pai = true;
  }
  
  return pai;
}

int main(){
  srand(time(NULL));

  ll tenho = 0;
  for (ll k = 1; k <= 330000000; k++){
    vector<ll> yama(136, 1), hai(39, 0), hai_show(14);
    vector<ll> yamakind = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 24, 25, 26, 27, 28, 29, 31, 32, 33, 34, 35, 36, 37};
    for (ll i = 0; i < 136; i++){
      yama[i] = yamakind[i / 4];
    }
    hai[38] = 99999;
    ll cnt = 0;
    while (cnt < 14){
      ll tar = rand() % 136;
      if (yama[tar] == 0) continue;

      hai[yama[tar]]++;
      hai_show[cnt] = yama[tar];
      yama[tar] = 0;
      cnt++;
    }
    sort(hai_show.begin(), hai_show.end());

    if (tenpai_ment(1, 0, 0, 0, hai) || tenpai_chitoi(1, 0, hai) || tenpai_kokusi(hai)){
      tenho++;
      cout << "TENHO!!!: ";
      for (ll i = 0; i < 14; i++){
        cout << hai_show[i] << ", ";
      }
      cout << endl;
    }

    if (k % 1000000 == 0){
      cout << "test: " << k << ' ' << "tenho: " << tenho << endl;
    }
  }
}
