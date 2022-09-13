#include <algorithm>
#include <vector>

/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in in[x + y*nx]
- for each pixel (x, y), store the median of the pixels (a, b) which satisfy
  max(x-hx, 0) <= a < min(x+hx+1, nx), max(y-hy, 0) <= b < min(y+hy+1, ny)
  in out[x + y*nx].
*/

void mf(int ny, int nx, int hy, int hx, const float *in, float *out) {
  int wind_hs;
  int wind_he;
  int wind_ws;
  int wind_we;
  
  for (int y=0; y < ny ; ++y) {
    for (int x=0 ; x < nx ; ++x){

      wind_hs = std::max(x-hx, 0); 
      wind_he = std::min(x+hx+1, nx);
      wind_ws = std::max(y-hy, 0);
      wind_we = std::min(y+hy+1, ny);
      std::vector<double> buf;

      for (int y1=wind_ws; y1 < wind_we ; ++y1) {
        for (int x1=wind_hs ; x1 < wind_he ; ++x1){
          buf.push_back(in[x1 + nx*y1]);
        }
      }
      auto m = buf.begin() + buf.size()/2;
      std::nth_element(buf.begin(), m, buf.end());
      
      if (buf.size()%2 == 0){
        auto m = buf.begin() + buf.size()/2-1;
        std::nth_element(buf.begin(), m, buf.end());
        out[x + nx*y] = float(buf[buf.size()/2] + buf[buf.size()/2-1])/2;
      } else {
        out[x + nx*y] = float(buf[buf.size()/2]);
      }
    }
  }
}
