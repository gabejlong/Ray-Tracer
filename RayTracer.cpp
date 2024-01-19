#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
using namespace std;

struct Vec 
{//datatype for 3D vectors
  double x;
  double y;
  double z;
  Vec(double x, double y, double z) : x(x), y(y), z(z) {}
  Vec operator - (const Vec& v) const { return Vec(x-v.x, y-v.y, z-v.z); }
  Vec operator + (const Vec& v) const { return Vec(x+v.x, y+v.y, z+v.z); }

  Vec sqrtVec()
  {
    return Vec(sqrt(x),sqrt(y),sqrt(z));
  }
  double dot(Vec a)//dot product
  {
    return (a.x*x+a.y*y+a.z*z);
  }
  Vec scal(double a)//multiplying a vec by a scalar
  {
    return Vec(x*a,y*a,z*a);
  }
  double magn()//vectors magnitude
  {
    return sqrt(x*x+y*y+z*z);
  }
  Vec normalize()
  {
    double length=sqrt(x*x+y*y+z*z);
    return Vec(x/length,y/length,z/length);
  }
  Vec clamp()//clamps rgb colors within 0 and 255
  {
    if (x<0) x=0;
    else if (x>255) x=255;

    if (y<0) y=0;
    else if (y>255) y=255;

    if (z<0) z=0;
    else if (z>255) z=255;
    return Vec(x,y,z);
  }
};
struct Ray
{
  Vec o;        //origin
  Vec d;        //direction
  Ray(Vec o, Vec d) : o(o), d(d) {}//constructor
};
struct Sphere
{
  Vec c;        //centre point
  double r;     //radius
  Vec col;      //rgb color of spehere
  Sphere(Vec c, double r, Vec col) : c(c), r(r), col(col) {}//constructor
  double intersect(Ray ray)//intersection with ray function
  {
    //easier referencing
    Vec o=ray.o;
    Vec d=ray.d;
    //use quadratic equation, a value is 1 because ray is normalized
    double B=(d.dot(o-c))*2;
    double C=(o-c).dot(o-c)-r*r;
    
    double disc=B*B-(C*4);//discriminant
    double t0=(-B+sqrt(disc))/2;//first intersection point
    double t1=(-B-sqrt(disc))/2;//second intersection point
    if (disc<0) return 0;//if the disc is negative doesn't intersect
    return (t1>0) ? t1 : (t0>0) ? t0 : 0;//find the smaller positive intersection point
  }
};

int main () 
{
  int W=400;//image width
  int H=400;//image height  

  vector <Sphere> spheres;//list of all spheres in scene
  int numofsph;//number of spheres
  cout<<"Enter how many spheres are in the scene."<<endl;
  cin>>numofsph;

  double ix;//for centre input x value
  double iy;//for centre input y value
  double iz;//for centre input z value
  double ir;//for sphere radius
  double ired;//for spheres red value
  double igre;//for spheres green value
  double iblu;//for spheres blue value
  for (int i=0; i<numofsph; i++)
  {
    cout<<"Enter the x (horizontal) coordinate for the centre.";
    cin>>ix;
    cout<<"Enter the y (vertical) coordinate for the centre.";
    cin>>iy;
    cout<<"Enter the z (depth) coordinate for the centre.";
    cin>>iz;
    cout<<"Enter the radius of the sphere.";
    cin>>ir;
    cout<<"Enter the red value of the sphere.";
    cin>>ired;
    cout<<"Enter the green value of the sphere.";
    cin>>igre;
    cout<<"Enter the blue value of the sphere.";
    cin>>iblu;
    spheres.push_back(Sphere(Vec(ix,iy,iz),ir,Vec(ired,igre,iblu)));
  }

  double lx;//for lights x value
  double ly;//for lights y value
  double lz;//for lights z value
  cout<<"What should the light source x (horizontal) coordinate be.";
  cin>>lx;
  cout<<"What should the light source y (vertical) coordinate be.";
  cin>>ly;
  cout<<"What should the light source z (depth) coordinate be.";
  cin>>lz;
  Sphere light(Vec(lx,ly,lz),1,Vec(255,255,255));

  double t=-1;//distance from camera to an object for each pixel negative value means no intersection
  int rs=-1;//which sphere in the vector the ray intersects with first
  Vec pix(0,0,0);//rgb vector for specific pixel
  
  ofstream myfile;
  myfile.open("output.ppm");
  myfile<<"P3"<<endl;//character required for ppm files
  myfile<<W<<" "<<H<<" 255"<<endl;//x and y limits, and rgb limit

  for (int y=0;y<H;y++)//runs for image height
  {
    for (int x=0; x<W;x++)//runs for image width
    {
      t=-1;//resets to no intersection
      rs=-1;//resets to no intersection
      pix=Vec(0,0,0);
      
      Ray cam(Vec(x,y,0), Vec(0,0,1));
      for (int i=0;i<spheres.size();i++)//finds the closest sphere that it intersects with and the distance from the camera
      {
        if (spheres[i].intersect(cam)>0 && t==-1)//if t has not been assigned yet but theres an intersection point assign t and rs
        {
          t=spheres[i].intersect(cam);
          rs=i;
        }
        else if (spheres[i].intersect(cam)<t && spheres[i].intersect(cam)>0)//if t has been assigned but this intersection point is closer reassign t and rs
        {
          t=spheres[i].intersect(cam);
          rs=i;
        }
      }
      if (t>0)//if there is an intersection then calculate the lighting
      {
        Vec POI=(cam.d).scal(t)+cam.o;//find the point of intersection
        Ray normal(POI,POI-spheres[rs].c);//find the normal at that point
        double bright=((light.c-POI).normalize()).dot(normal.d.normalize());//calculate the dot product of normal and light ray
        pix=(light.col.scal(bright)+spheres[rs].col).scal(0.5);//affect the pixel according to bright(this is because bigger bright values mean a more direct angle which means brighter)
        pix=pix.clamp();//clamp the pixel color so the color doesn't blow out
      }
      myfile<<(int)pix.x<<" "<<(int)pix.y<<" "<<(int)pix.z<<endl;//output the pixel values to the file in ppm format
    }
  }
  return 0;
}
