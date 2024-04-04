#ifndef WAVE_H
#define WAVE_H

#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include <cstdlib> // For random number generation
#include <ctime> 
#include <random>

using namespace std;

typedef struct // Definition of a terrain point
{
    GLfloat s, t; // Texture coordinates
    GLfloat nx, ny, nz; // Normal coordinates
    GLfloat x, height, z; // Vertex position
} WPoint;


class Wave
{
public:
    GLfloat amplitude;
    GLfloat frequency;
    GLfloat direction;
    GLfloat phase;
    Wave(GLfloat amplitude, GLfloat frequency, GLfloat direction, GLfloat phase) {
        this->amplitude = amplitude;
        this->frequency = frequency;
        this->direction = direction;
        this->phase = phase;
    }
};


class Ocean
{
public:
    GLuint n_x, m_z;
    GLfloat size_x, size_z;
    int n_waves;
    WPoint *points;
    vector<GLuint> list_index;
    GLuint *pointer_list_index;
    vector<Wave> waves;

    GLfloat computeHeight(float x, float z, float t)
    {
        GLfloat height = 0.0;
        GLfloat k;
        for( int i=0; i< waves.size(); i++ )
        {
            k = 4.0 * M_PI * M_PI * waves[i].frequency * waves[i].frequency / 9.81;
            height += waves[i].amplitude * cos(k*(x*cos(waves [i].direction)+z*sin(waves[i].direction))-2.0f*M_PI* waves [i].frequency*t + waves [i].phase );
        }
        return height;
    }

    void update_heights(float t){
        for(int i=0; i<(m_z*n_x); i++){
            GLfloat h_temp = computeHeight(this->points[i].x, this->points[i].z, t);
            this->points[i].height = abs(h_temp)/2.5;
        }
    }


    Ocean(GLuint n_x, GLuint m_z, GLfloat size_x, GLfloat size_z){
        this->n_x = n_x;
        this->m_z = m_z;
        this->size_x = size_x;
        this->size_z = size_z;
    }
// waves as small pools.

void load_waves(string file) {
  std::ifstream infile(file.c_str());
  std::string line;
  
  // Seed the random number generator (optional for better randomness)
  static std::default_random_engine generator(std::random_device{}());
  static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    GLfloat a, b, c;
    if (!(iss >> a >> b >> c)) { break; } // error

    // Generate random factor within a specific range (adjust as needed)
    float randomnessFactor = distribution(generator);

    // // Add randomness to each value
    a += (randomnessFactor * 0.2f - 0.1f);  // Adjust factor and range here
    b += (randomnessFactor * 0.2f - 0.1f);
    c += (randomnessFactor * 0.2f - 0.1f);

    Wave f(a, b, c, 0);
    waves.push_back(f);
  }
}


    void create_list_vertex(){
        this->points = new WPoint[n_x*m_z];
        int indexList = 0;
        for(GLfloat z=0.0; z<m_z; z += size_z){
            for(GLfloat x=0.0; x<n_x; x += size_x){
                this->points[indexList].x = x;
                this->points[indexList].height = 1;
                this->points[indexList].z = z;

                this->points[indexList].nx = 0;
                this->points[indexList].ny = 0;
                this->points[indexList].nz = 1;

                this->points[indexList].s = 1;
                this->points[indexList].t = 1;
                indexList++;
            }
        }
    }

void create_index_list(){
        GLuint size_list = (n_x*m_z)*3;
        int index_list = 0;
        GLuint index_pos = n_x;
        for(GLuint z=1; z<m_z; z++){
            GLuint old_index_pos = index_pos;
            for(GLuint x=0; x<n_x-1; x++){
                this->list_index.push_back(index_pos);
                this->list_index.push_back(index_pos-(n_x-1));
                this->list_index.push_back((index_pos-(n_x-1))-1);

                this->list_index.push_back(index_pos + 1);
                this->list_index.push_back((index_pos+1)-(n_x));
                this->list_index.push_back(index_pos);
                index_pos += 1;
            }
            index_pos = old_index_pos;
            index_pos += n_x;
        }
        this->pointer_list_index = &(this->list_index[0]);
    }

    void display(){
        glInterleavedArrays(GL_T2F_N3F_V3F, sizeof(WPoint), &this->points[0].s);
        glDrawElements(GL_TRIANGLES, this->list_index.size() , GL_UNSIGNED_INT, (void*)this->pointer_list_index);
        glDisable(GL_COLOR_MATERIAL);
    }
};


#endif // WAVE_H
    // # this is waves as waves.
    // void load_waves(const std::string& file) {
    //     std::ifstream infile(file);
    //     if (!infile.is_open()) {
    //         std::cerr << "Error: Unable to open file " << file << std::endl;
    //         return;
    //     }

    //     // Random number generator for introducing variability
    //     std::random_device rd;
    //     std::mt19937 gen(rd());
    //     std::uniform_real_distribution<GLfloat> disAmplitude(1.0, 3.0); // Vary amplitude between 1 and 3
    //     std::uniform_real_distribution<GLfloat> disFrequency(0.01, 0.03); // Vary frequency between 0.01 and 0.03
    //     std::uniform_real_distribution<GLfloat> disPhase(0.0, 2 * M_PI); // Randomize phase between 0 and 2pi

    //     std::string line;
    //     while (std::getline(infile, line)) {
    //         std::istringstream iss(line);
    //         GLfloat length, height;
    //         if (!(iss >> length >> height)) {
    //             std::cerr << "Error: Invalid data format in file " << file << std::endl;
    //             break;
    //         }

    //         // Introduce variability in wave properties
    //         GLfloat amplitude = disAmplitude(gen);
    //         GLfloat frequency = disFrequency(gen);
    //         GLfloat phase = disPhase(gen);

    //         // Calculate wave parameters based on panel length and height
    //         GLfloat waveAmplitude = height / 2.0; // Half of the panel height
    //         GLfloat waveFrequency = M_PI / length; // Wave frequency inversely proportional to panel length

    //         Wave wave(amplitude * waveAmplitude, frequency * waveFrequency, 0.0, phase);
    //         waves.push_back(wave);
    //     }
    //     infile.close();
    // }