#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <vector>
#include <queue>

using namespace std;
using namespace cv;

Mat original_image;
Mat modified_image;
int thresh;

void negCovert(Mat* image)
{
    int numPixels = image->rows * image->step;
    for(int i = 0; i < numPixels; i++)
    {
        //modified_image.data[i] = 0;
        modified_image.data[i] = 255 - modified_image.data[i];
    }
    
    vector<int> hist1(256, 0);
    vector<int> hist2(256, 0);
    for(int i = 0; i < numPixels; i++)
    {
        hist1[image->data[i]]++;
    }
    for(int i = 0; i < numPixels; i++)
    {
        hist2[modified_image.data[i]]++;
    }
    cout << "\nHistogram values for original image: " << endl;
    for (auto i = hist1.begin(); i != hist1.end(); ++i)
    {
        cout << *i << ' ';
    }
    cout << "\n\n";
    cout << "Histogram values for modified image: " << endl;
    for (auto i = hist2.begin(); i != hist2.end(); ++i)
    {
        cout << *i << ' ';
    }
    cout << "\n";
    
}

void binConvert(Mat* image) {
    int numPixels = image->rows * image->step;
    vector<int> hist1(256, 0);
    vector<int> hist2(256, 0);
    for(int i = 0; i < numPixels; i++)
    {
        hist1[image->data[i]]++;
    }
    // Calculate threshold
    int threshold = 0;
    for(int i = 0; i < hist1.size(); i++) {
        threshold += i * hist1[i];
    }
    threshold /= numPixels;
    thresh = threshold;
    for(int i = 0; i < numPixels; i++) {
        modified_image.data[i] = modified_image.data[i] < threshold ? 0 : 255;
    }
    for(int i = 0; i < numPixels; i++)
    {
        hist2[modified_image.data[i]]++;
    }
    cout << "\nHistogram values for original image: " << endl;
    for (auto i = hist1.begin(); i != hist1.end(); ++i)
    {
        cout << *i << ' ';
    }
    cout << "\n\n";
    cout << "Histogram values for modified image: " << endl;
    for (auto i = hist2.begin(); i != hist2.end(); ++i)
    {
        cout << *i << ' ';
    }
    cout << "\n";
}

void equalConvert(Mat* image)
{
    int numPixels = image->rows * image->step;
    vector<int> hist1(256, 0);
    vector<int> hist2(256, 0);
    for(int i = 0; i < numPixels; i++)
    {
        hist1[image->data[i]]++;
    }
    int sum = 0;
    for(int i = 0; i < hist1.size(); i++) {
        sum += hist1[i];
        hist2[i] = (sum * 255) / numPixels;
    }
    for(int i = 0; i < numPixels; i++) {
        modified_image.data[i] = hist2[modified_image.data[i]];
    }
    cout << "\nHistogram values for original image: " << endl;
    for (auto i = hist1.begin(); i != hist1.end(); ++i)
    {
        cout << *i << ' ';
    }
    cout << "\n\n";
    cout << "Histogram values for modified image: " << endl;
    for (auto i = hist2.begin(); i != hist2.end(); ++i)
    {
        cout << *i << ' ';
    }
    cout << "\n";
}

void findRegions(Mat* image) {
    binConvert(image);
    int shade = 0;
    int numPixels = modified_image.rows * modified_image.step;
    int width = image->size().width;
    queue<int> toVisit;
    vector<bool> visited(numPixels, false);
    vector<vector<int> > regions;
    bool foundNewRegion = false;
    
    for(int i = 0; i < numPixels; i++) {
        vector<int> region;
        if(!visited[i] && modified_image.data[i] == 255) {
            toVisit.push(i);
            visited[i] = true;
            shade = (shade + 50) % 255;
            foundNewRegion = true;
        }
        while(!toVisit.empty()) {
            int pix = toVisit.front(); toVisit.pop();
            region.push_back(pix);
            modified_image.data[pix] = 120;
            int left = pix - 1,
            right = pix + 1,
            up = pix - width,
            down = pix + width;
            
            if(left % width != 0 && !visited[left] && modified_image.data[left] == 255) {
                toVisit.push(left);
                visited[left] = true;
            }
            if(right % width != 0 && !visited[right] && modified_image.data[right] == 255) {
                toVisit.push(right);
                visited[right] = true;
            }
            if(up >= 0 && !visited[up] && modified_image.data[up] == 255) {
                toVisit.push(up);
                visited[up] = true;
            }
            if(down < numPixels && !visited[down] && modified_image.data[down] == 255) {
                toVisit.push(down);
                visited[down] = true;
            }
        }
        if(foundNewRegion) {
            regions.push_back(region);
            foundNewRegion = false;
        }
    }
    //cout << "Got here" << endl;
    regions.erase(remove_if(regions.begin(), regions.end(),[](const vector<int> &a)
    {
        return a.size() < 50;
    }),
    regions.end());
    
    sort(regions.begin(), regions.end(), [](const vector<int> &a, const vector<int> &b)
    {
        return a.size() < b.size();
    });
    if(regions.size() <= 0) return;
    cout << regions.size() << " regions detected!\n";
    
    for(int i = 1; i <= regions.size(); i++) {
        cout << "Region " << i << " has " << regions[i-1].size() << " pixels\n";
    }
    
    cout << "Smallest region: " << regions.front().size() << " | Largest region: " << regions.back().size() << endl;
    cout << "The threshold value is " << thresh << endl;
    
    for(int i = 0; i < regions.front().size(); i++) {
        modified_image.data[regions.front()[i]] = 60;
    }
    
    for(int i = 0; i < regions.back().size(); i++) {
        modified_image.data[regions.back()[i]] = 200;
    }
}

int main(int argc, char **argv) {
  
  if(argc != 3) {
    cout << "USAGE: skeleton <input file path> <conversion type>" << endl;
    return -1;
  }
  
  //Load two copies of the image. One to leave as the original, and one to be modified.
  //Done for display purposes only
  //Use CV_LOAD_IMAGE_GRAYSCALE for greyscale images
  original_image = imread(argv[1], IMREAD_ANYCOLOR);
  modified_image = imread(argv[1], IMREAD_ANYCOLOR);
  
  //Create a pointer so that we can quickly toggle between which image is being displayed
  Mat *image = &original_image;
 
  //Check that the images loaded
  if(!original_image.data || !modified_image.data) {
    cout << "ERROR: Could not load image data." << endl;
    return -1;
  }
  
  //Create the display window
  namedWindow("Unix Sample Skeleton");
  
  //Replace center third of the image with white
  //This can be replaced with whatever filtering you need to do.
  //size_t offset1 = image->rows/3 * image->step;
  //size_t offset2 = image->rows/3*2 * image->step;
    if(strncmp(argv[2], "negative", 10) == 0 || strncmp(argv[2], "Negative", 10) == 0)
    {
        negCovert(image);
    }
    else if(strncmp(argv[2], "binary", 10) == 0 || strncmp(argv[2], "Binary", 10) == 0)
    {
        binConvert(image);
    }
    else if(strncmp(argv[2], "equalize", 10) == 0 || strncmp(argv[2], "Equalize", 10) == 0)
    {
        equalConvert(image);
    }
    else if(strncmp(argv[2], "regions", 10) == 0 || strncmp(argv[2], "Regions", 10) == 0)
    {
        findRegions(image);
    }
    else
    {
        cout << "ERROR: Not proper conversion type" << endl;
        return -1;
    }
  //Display loop
  bool loop = true;
  while(loop) {
    imshow("Unix Sample Skeleton", *image);
    
    switch(waitKey(15)) {
      case 27:  //Exit display loop if ESC is pressed
        loop = false;
        break;
      case 32:  //Swap image pointer if space is pressed
        if(image == &original_image) image = &modified_image;
        else image = &original_image;
        break;
      default:
        break;
    }
  }
}
