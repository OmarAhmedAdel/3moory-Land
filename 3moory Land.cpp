#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <glut.h>
#include <time.h>
#include <string.h>
#include <cmath>
#include <playsoundapi.h>
#include <iostream>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

#define M_PI 3.14159265358979323846



bool omarAdel = false;

float playerZ = 0.0; // movement forward and backward
float playerX = 0.0; // movement left and right
float playerY = 0.2; // movement up and down

float playerRotation = 0.0; // Initial rotation angle

int lastColorChangeTime = 0; // Global variable to store the last time the color was changed
bool isChangingColor = false; // Boolean flag to indicate whether the fence is currently changing color
bool changeColor = false;

float ferrisWheelRotation = 0.0;

float seesawRotation = 0.0;

float chineseBalloonRotation = 0.0;

float jackRotation = 0.0;

float jackPartRotation = 0.0;

bool pauseAnimations = false;  // Boolean flag to indicate whether the animations are paused

const float collisionThreshold = 0.15; // Define a small threshold for collision detection

int remainingTime = 120;  // Initial time in seconds
int remainingMinutes = remainingTime / 60;  // Calculate remaining minutes
int remainingSeconds = remainingTime % 60;  // Calculate remaining seconds
bool isGameOver = false;  // Boolean flag to indicate whether the game is over

float basketBallRotation = 0.0;

float swingRotation = 0.0;




bool gameSound = false;
bool winSound = false;
bool loseSound = false;


int windowWidth = 640;
int windowHeight = 480;



class Vector3f {
public:
    float x, y, z;

    Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
        : x(_x), y(_y), z(_z) {}

    Vector3f operator+(const Vector3f& v) const {
        return Vector3f(x + v.x, y + v.y, z + v.z);
    }

    Vector3f operator-(const Vector3f& v) const {
        return Vector3f(x - v.x, y - v.y, z - v.z);
    }

    Vector3f operator*(float n) const {
        return Vector3f(x * n, y * n, z * n);
    }

    Vector3f operator/(float n) const {
        return Vector3f(x / n, y / n, z / n);
    }

    Vector3f unit() const {
        float magnitude = sqrt(x * x + y * y + z * z);
        return *this / magnitude;
    }

    Vector3f cross(const Vector3f& v) const {
        return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
};

class Camera {
public:
    Vector3f eye, center, up;

    Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f,
        float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f,
        float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f)
        : eye(eyeX, eyeY, eyeZ), center(centerX, centerY, centerZ), up(upX, upY, upZ) {}

    void moveX(float d) {
        Vector3f right = up.cross(center - eye).unit();
        eye = eye + right * d;
        center = center + right * d;
    }

    void moveY(float d) {
        eye = eye + up.unit() * d;
        center = center + up.unit() * d;
    }

    void moveZ(float d) {
        Vector3f view = (center - eye).unit();
        eye = eye + view * d;
        center = center + view * d;
    }

    void rotateX(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
        up = view.cross(right);
        center = eye + view;
    }

    void rotateY(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
        right = view.cross(up);
        center = eye + view;
    }

    void look(const Vector3f& position) const {
        gluLookAt(
            eye.x, eye.y, eye.z,
            position.x, position.y, position.z,
            up.x, up.y, up.z
        );
    }
};

Camera camera;

void drawCylinder(float radius, float height, int segments) {
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float theta = (2.0f * M_PI * float(i)) / float(segments);
        float x = radius * cos(theta);
        float y = radius * sin(theta);

        glVertex3f(x, y, 0.0);
        glVertex3f(x, y, height);
    }
    glEnd();

    // Draw the top and bottom circles
    glBegin(GL_POLYGON);
    for (int i = 0; i <= segments; ++i) {
        float theta = (2.0f * M_PI * float(i)) / float(segments);
        float x = radius * cos(theta);
        float y = radius * sin(theta);

        glVertex3f(x, y, height);
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (int i = 0; i <= segments; ++i) {
        float theta = (2.0f * M_PI * float(i)) / float(segments);
        float x = radius * cos(theta);
        float y = radius * sin(theta);

        glVertex3f(x, y, 0.0);
    }
    glEnd();
}

void drawCharacter() {
    glPushMatrix();
    glTranslatef(0, 0.2, 0);

    // Draw head (Sphere)
    glPushMatrix();
    glColor3f(1.0, 0.8, 0.6);  // Skin color
    glTranslatef(0.17 + playerX, 0.0, playerZ);
    glutSolidSphere(0.1, 15, 15);
    glPopMatrix();

    // Draw body (Cube)
    glPushMatrix();
    glColor3f(0.0, 0.5, 1.0);  // Shirt color
    glTranslatef(0.17 + playerX, -0.2, playerZ);
    glutSolidCube(0.2);
    glPopMatrix();

    // Draw right arm (Cylinder)
    glPushMatrix();
    glColor3f(1.0, 0.8, 0.6);  // Skin color
    glTranslatef(0.15 + playerX, -0.2, playerZ);
    glRotatef(90, 0.0, 1.0, 0.0);  // Rotate the arm
    drawCylinder(0.05, 0.3, 15);
    glPopMatrix();

    // Draw left arm (Cylinder)
    glPushMatrix();
    glColor3f(1.0, 0.8, 0.6);  // Skin color
    glTranslatef(-0.15 + playerX, -0.2, playerZ);
    glRotatef(90, 0.0, 1.0, 0.0);  // Rotate the arm
    drawCylinder(0.05, 0.3, 15);
    glPopMatrix();

    // Draw legs (Cylinders)
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);  // Pants color

    // Draw right leg
    glPushMatrix();
    glTranslatef(0.22 + playerX, -0.3, playerZ);
    glRotatef(90, 1.0, 0.0, 0.0);
    drawCylinder(0.04, 0.3, 15);
    glPopMatrix();

    // Draw left leg
    glPushMatrix();
    glTranslatef(0.12 + playerX, -0.3, playerZ);
    glRotatef(90, 1.0, 0.0, 0.0);
    drawCylinder(0.04, 0.3, 15);
    glPopMatrix();

    glPopMatrix();

    // Draw hat (Cone)
    glPushMatrix();
    glColor3f(0.5, 0.0, 0.5);  // Hat color
    glTranslatef(0.17 + playerX, 0.1, playerZ);
    glRotatef(-90, 1.0, 0.0, 0.0);  // Rotate the cone upside down
    glutSolidCone(0.08, 0.1, 15, 15);
    glPopMatrix();

    glPopMatrix();
}

void drawGround() {
    // Draw ground
    glPushMatrix();
    glColor3f(0.0, 1.0, 0.0);  // Green color for the ground
    glBegin(GL_QUADS);
    glVertex3f(-2.0, -0.4, -2.0);
    glVertex3f(2.0, -0.4, -2.0);
    glVertex3f(2.0, -0.4, 2.0);
    glVertex3f(-2.0, -0.4, 2.0);
    glEnd();
    glPopMatrix();
}





void rotatePlayer(float angle) {
    playerRotation = angle;
    glutPostRedisplay();  // Trigger a redisplay to update the rendering
}



// Function to handle color change
void updateColor(int value) {
    // Toggle the color change state
    changeColor = !changeColor;

    // Set up the next timer call (e.g., 5000 milliseconds for 5 seconds)
    glutTimerFunc(5000, updateColor, 0);
}

void drawFence() {

    //glColor3f(0.5, 0.3, 0.0);  // Brown color for the fence
    float fenceHeight = 1.0;
    float fenceWidth = 0.05;   // Width of each fence picket
    float fenceLength = 4.0;  // Length of the fence
    float gapBetweenPickets = 0.1;  // Gap between each picket

    int numberOfPickets = static_cast<int>((fenceLength - gapBetweenPickets) / (fenceWidth + gapBetweenPickets));

    // Draw on the front side
    for (int i = 0; i < numberOfPickets; ++i) {
        float x = -2.0 + i * (fenceWidth + gapBetweenPickets);

        // Set the color based on the changeColor state
        if (changeColor) {
            glColor3f(0.2, 0.6, 0.8);  // Example color (adjust as needed)
        }
        else {
            glColor3f(0.5, 0.3, 0.0);  // Brown color for the fence
        }

        glBegin(GL_QUADS);
        glVertex3f(x, -0.4, -2.0);
        glVertex3f(x + fenceWidth, -0.4, -2.0);
        glVertex3f(x + fenceWidth, fenceHeight, -2.0);
        glVertex3f(x, fenceHeight, -2.0);
        glEnd();

        // Draw cone on top
        glColor3f(0.8, 0.4, 0.1);  // Color for the cone
        glPushMatrix();
        glTranslatef(x + fenceWidth / 2, fenceHeight + 0.1, -2.0);  // Adjust height of the cone
        glRotatef(83.0, 1.0, 0.0, 0.0);  // Rotate the cone to stand on top 
        glutSolidCone(0.1, 0.2, 20, 20);
        glPopMatrix();
    }

    // Draw on the right side
    for (int i = 0; i < numberOfPickets; ++i) {
        float z = -2.0 + i * (fenceWidth + gapBetweenPickets);

        // Set the color based on the changeColor state
        if (changeColor) {
            glColor3f(0.2, 0.6, 0.8);  // Example color (adjust as needed)
        }
        else {
            glColor3f(0.5, 0.3, 0.0);  // Brown color for the fence
        }

        glBegin(GL_QUADS);
        glVertex3f(2.0, -0.4, z);
        glVertex3f(2.0, -0.4, z + fenceWidth);
        glVertex3f(2.0, fenceHeight, z + fenceWidth);
        glVertex3f(2.0, fenceHeight, z);
        glEnd();

        // Draw cone on top
        glColor3f(0.8, 0.4, 0.1);  // Color for the cone
        glPushMatrix();
        glTranslatef(2.0, fenceHeight + 0.15, z + fenceWidth / 2);  // Adjust height of the cone
        glRotatef(90.0, 1.0, 0.0, 0.0);  // Rotate the cone to stand on top 
        glutSolidCone(0.1, 0.2, 20, 20);
        glPopMatrix();
    }

    // Draw on the left side
    for (int i = 0; i < numberOfPickets; ++i) {
        float z = -2.0 + i * (fenceWidth + gapBetweenPickets);

        // Set the color based on the changeColor state
        if (changeColor) {
            glColor3f(0.2, 0.6, 0.8);  // Example color (adjust as needed)
        }
        else {
            glColor3f(0.5, 0.3, 0.0);  // Brown color for the fence
        }

        glBegin(GL_QUADS);
        glVertex3f(-2.0, -0.4, z);
        glVertex3f(-2.0, -0.4, z + fenceWidth);
        glVertex3f(-2.0, fenceHeight, z + fenceWidth);
        glVertex3f(-2.0, fenceHeight, z);
        glEnd();

        // Draw cone on top
        glColor3f(0.8, 0.4, 0.1);  // Color for the cone
        glPushMatrix();
        glTranslatef(-2.0, fenceHeight + 0.1, z + fenceWidth / 2);  // Adjust height of the cone
        glRotatef(90.0, 1.0, 0.0, 0.0);  // Rotate the cone to stand on top
        glutSolidCone(0.1, 0.2, 20, 20);
        glPopMatrix();
    }
}

void drawFerrisWheel() {
    // Increment the rotation angle for each frame
    ferrisWheelRotation += 0.5;  // You can adjust the rotation speed as needed

    glPushMatrix();
                glTranslated(1.0, 1.50, 1.0);  // Move the Ferris Wheel to the back //TODO: change the position
        glRotated(-90, 1, 0, 0);
        glTranslatef(-2.0, -0.5, -0.8);  // Move the Ferris Wheel to the back
        //glRotatef(ferrisWheelRotation, 0.0, 0.0, 1.0);  // Apply the rotation
        // Draw Ferris Wheel Hub (Cube)
        glColor3f(0.7, 0.3, 0.5);  // Gray color for the hub
        glPushMatrix();
        glTranslatef(0.0, 0.1, -0.6);  // Move to the top of the hub
        glScaled(.5, .5, 6.0);  // Scale to make the hub shorter
        glutSolidCube(0.2);  // Hub cube
        glPopMatrix();

        // Draw Ferris Wheel spokes and seats (Cubes)
        glColor3f(0.7, 0.3, 0.5);  // Light gray color for spokes and seats
        int numSpokes = 8;  // Number of spokes
        float wheelRadius = 0.8;  // Radius of the Ferris Wheel
        float angleIncrement = 360.0 / numSpokes;

        for (int i = 0; i < numSpokes; ++i) {
            float angle = i * angleIncrement;

            // Draw spoke
            glPushMatrix();
            glRotatef(angle + ferrisWheelRotation, 0.0, 0.0, 1.0);
            glutSolidCube(0.02);  // Cube representing a spoke
            glPopMatrix();

            // Draw seat
            glPushMatrix();
            glRotatef(angle + 90.0 + ferrisWheelRotation, 0.0, 1.0, 0.0);
            glTranslatef(0.0, -0.1, wheelRadius);
            glRotatef(ferrisWheelRotation * 2, 0.0, 0.0, 1.0);  // Rotate the seat
            glutSolidCube(0.1);  // Cube representing a hanging seat
            glPopMatrix();
        }
    glPopMatrix();
}

void updateFerrisWheelRotation(int value) {
    // Increment the rotation angle for each frame
    if (!pauseAnimations) {
        ferrisWheelRotation += 2.0;  // You can adjust the rotation speed as needed

        // Set up the next timer call (e.g., 50 milliseconds for smoother rotation)
        glutTimerFunc(50, updateFerrisWheelRotation, 0);

        // Trigger a redisplay to show the updated Ferris Wheel rotation
        glutPostRedisplay();
    }
}



void drawSeeSaw(float movement) {
    // Dimensions of the seesaw
    float plankWidth = 0.1;
    float plankLength = 1.5;
    float plankThickness = 0.02;
    float springHeight = 0.5;
    float springRadius = 0.02;
    float triangleHeight = 0.2;
    int segments = 20;
    glPushMatrix();
    glTranslatef(0.0, -0.21,0);


    glPushMatrix();
    glTranslatef(movement, 0.0, 0.3);  // Apply movement
    glRotatef(seesawRotation, 0.0, 0.0, 1.0);  // Apply seesaw rotation
    // Draw left spring
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);  // Gray color for the spring
    glTranslatef(-plankLength / 2 + movement, -springRadius+ 0.025, -0.25);
    drawCylinder(springRadius, springHeight, segments);
    glPushMatrix();
    glTranslatef(0,0,0.25);
    glutSolidCube(0.1);
    glPopMatrix();
    glPopMatrix();

    // Draw right spring
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);  // Gray color for the spring
    glTranslatef(plankLength / 2 + movement, -springRadius+0.025, -0.25);
    drawCylinder(springRadius, springHeight, segments);
    glPushMatrix();
    glTranslatef(0, 0, 0.25);
    glutSolidCube(0.1);
    glPopMatrix();
    glPopMatrix();

    // Draw plank
    glPushMatrix();
    glColor3f(0.9, 0.6, 0.3);  // Light brown color for the plank
    glTranslatef(0.0, -springRadius+0.04, 0.0);
    //glRotatef(seesawRotation, 0.0, 0.0, 1.0);  // Apply seesaw rotation
    glScalef(plankLength, plankThickness, plankWidth);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();
    // Draw triangle support
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);  // Gray color for the triangle
    glRotated(-90,1,0,0);
    glTranslatef(0.0, ( - springRadius - plankThickness / 2) - 0.3 , -0.4);
    glBegin(GL_TRIANGLES);
    glutSolidCone(0.1, 0.2, 20, 20);
    /*
    glVertex3f(-plankLength / 2 - triangleHeight / 2, 0.0, 0.0);
    glVertex3f(plankLength / 2 + triangleHeight / 2, 0.0, 0.0);
    glVertex3f(0.0, triangleHeight, 0.0);
    */
    glEnd();
    glPopMatrix();
}


void updateSeeSawMovement(int value) {
    // Function to update the seesaw's movement
    if (!pauseAnimations) {
        // The seesaw oscillates back and forth, so we use sine function
        float movement = 0.2 * sin(glutGet(GLUT_ELAPSED_TIME) / 1000.0);
        seesawRotation = movement * 45.0;  // Adjust the multiplier for the desired rotation angle

        glutPostRedisplay();
        glutTimerFunc(50, updateSeeSawMovement, 0);
    }
}


void drawChineseBalloon() {
    
    glPushMatrix();
    glRotated(chineseBalloonRotation, 0, 0, 1);
    // Balloon body
    glPushMatrix();
    glTranslatef(0.0, 0.05, -1.5);
    glColor3f(1.0, 0.4, 0.4);  // Light red color for the balloon body
    glutSolidSphere(0.2, 20, 20);
    glPopMatrix();

    // Balloon top
    glPushMatrix();
    glTranslatef(0.0, -0.15, -1.5);
    glColor3f(1.0, 1.0, 0.0);  // Yellow color for the top
    glutSolidSphere(0.1, 20, 20);
    glPopMatrix();

    // Balloon tail
    glColor3f(0.0, 0.4, 1.0);  // Light blue color for the tail
    glPushMatrix();
    glTranslatef(0.0, -0.15, -1.5);
    glRotatef(90, 1.0, 0.0, 0.0);
    glutSolidCone(0.05, 0.4, 10, 10);
    glPopMatrix();

    glPopMatrix();
}

void updateChineseBalloonRotation(int value) {
	// Function to update the Chinese balloon's rotation
    if (!pauseAnimations) {
     		chineseBalloonRotation += 5.0;  // You can adjust the rotation speed as needed
     
     		glutPostRedisplay();
     		glutTimerFunc(50, updateChineseBalloonRotation, 0);
     }
 

}


/*// Function to check collision between the character and Chinese balloon
bool checkGoalCollision() {
    // Assuming the bounding boxes of the character and balloon for simplicity
    float characterMinX = 0.17 + playerX - 0.1;
    float characterMaxX = 0.17 + playerX + 0.1;
    float characterMinY = -0.2 + playerY - 0.3;
    float characterMaxY = -0.2 + playerY + 0.3;
    float characterMinZ = playerZ - 0.1;
    float characterMaxZ = playerZ + 0.1;

    // Assuming the bounding box of the balloon for simplicity
    float balloonMinX = -0.2;
    float balloonMaxX = 0.2;
    float balloonMinY = -0.25;
    float balloonMaxY = 0.05;
    float balloonMinZ = -1.7;
    float balloonMaxZ = -1.3;

    // Check for collision
    if (characterMaxX > balloonMinX && characterMinX < balloonMaxX &&
        characterMaxY > balloonMinY && characterMinY < balloonMaxY &&
        characterMaxZ > balloonMinZ && characterMinZ < balloonMaxZ) {
        return true;  // Collision detected
    }

    return false;  // No collision
}*/


// Function to check if the player has collided with the balloon
/*bool isGoalCollision() {
    // Define the position of the balloon
    float balloonX = 0.0;  // Set the X coordinate of the balloon
    float balloonY = 0.05;  // Set the Y coordinate of the balloon
    float balloonZ = -1.5;  // Set the Z coordinate of the balloon

    // Define the collision radius (adjust as needed)
    float collisionRadius = 0.3;

    // Calculate the distance between the player and the balloon
    float distance = std::sqrt(std::pow(playerX - balloonX, 2) + std::pow(playerY - balloonY, 2) + std::pow(playerZ - balloonZ, 2));

    // Check if the distance is within the collision radius
    if (distance < collisionRadius) {
        return true;  // Collision detected
    }
    else {
        return false;  // No collision
    }
}*/

// create a function to check if the player has collided with the balloon
bool checkGoalCollision() {

   // omarAdel = true;

    // Get the current position of the balloon's body
    float balloonX = -0.05;
    float balloonY = 0.05;
    float balloonZ = -0.75;

    

    // Define player and balloon radii
    float playerRadius = 0.1;  // Adjust this value based on your player's size
    float balloonRadius = 0.2; // Adjust this value based on your balloon's size

    //printf("%f %f\n", playerX, playerZ );
    // Calculate the distance between the player and the balloon in 3D space
    //float distance = sqrt(pow(playerX - balloonX, 2) +
       // pow(playerY - balloonY, 2) +
      //  pow(playerZ - balloonZ, 2));

    return omarAdel = ((playerX >= -0.6)) && (playerX <= 0.0) && ((playerZ >= -1.6) && (playerZ <= -1.35));
        

    //bool m = (distance - 0.4) < (playerRadius + balloonRadius);
    //printf("%f %f %d \n",distance - 0.4, playerRadius + balloonRadius, m);

    // Check if the distance is less than the sum of the player and balloon radii
    // If true, it means they are colliding
    //return (distance - 0.4) < (playerRadius + balloonRadius);
    glutPostRedisplay();
}




void drawTableLeg(double thick, double len) {

    glColor3f(1.0, 0.0, 0.0);  // Color for legs


    glPushMatrix();
    glTranslated(0, len / 2, 0);
    glScaled(thick, len, thick);
    glutSolidCube(1.0);
    glPopMatrix();

}

void drawJackPart() {

    glColor3f(1.0, 0.0, 0.0);  // Color for jack part

    glPushMatrix();
    glScaled(0.2, 0.2, 1.0);
    glutSolidSphere(1, 15, 15);
    glPopMatrix();
    glPushMatrix();
    glTranslated(0, 0, 1.2);
    glutSolidSphere(0.2, 15, 15);
    glTranslated(0, 0, -2.4);
    glutSolidSphere(0.2, 15, 15);
    glPopMatrix();
}
void drawJack() {

    glColor3f(1.0, 0.0, 0.0);  // Color for jack

    glPushMatrix();
    glRotated(jackRotation, 0, 0, 1);

    glPushMatrix();
    drawJackPart();
    glRotated(90.0, 0, 1, 0);
    drawJackPart();
    glRotated(90.0, 1, 0, 0);
    drawJackPart();
    glPopMatrix();

    glPopMatrix();
}

void updateJackRotation(int value) {
	// Function to update the jack's rotation
    if (!pauseAnimations) {
		jackRotation += 5.0;  // You can adjust the rotation speed as needed

		glutPostRedisplay();
		glutTimerFunc(50, updateJackRotation, 0);
	}
}

void drawTable(double topWid, double topThick, double legThick, double legLen) {

    glColor3f(1.0, 0.0, 0.0);  // Color for table


    glPushMatrix();
    glTranslated(0, legLen, 0);
    glScaled(topWid, topThick, topWid);
    glutSolidCube(1.0);
    glPopMatrix();

    double dist = 0.95 * topWid / 2.0 - legThick / 2.0;
    glPushMatrix();
    glTranslated(dist, 0, dist);
    drawTableLeg(legThick, legLen);
    glTranslated(0, 0, -2 * dist);
    drawTableLeg(legThick, legLen);
    glTranslated(-2 * dist, 0, 2 * dist);
    drawTableLeg(legThick, legLen);
    glTranslated(0, 0, -2 * dist);
    drawTableLeg(legThick, legLen);
    glPopMatrix();

}


void drawBasketballGoal(double poleHeight, double poleRadius, double backboardWidth, double backboardHeight, double hoopRadius) {
    
    glPushMatrix();
    glTranslated(1, -0.4, 0);
    glRotated(basketBallRotation, 0, 1, 0);
    
    // Draw pole
    glColor3f(0.5, 0.5, 0.5); // Gray color for the pole
    glPushMatrix();
    glTranslated(0, poleHeight / 2, 0);
    glScaled(poleRadius, poleHeight, poleRadius);
    glutSolidCube(1.0);
    glPopMatrix();

    // Draw backboard
    glColor3f(0.8, 0.8, 0.8); // Light gray color for the backboard
    glPushMatrix();
    glTranslated(0, poleHeight + backboardHeight / 2, 0);
    glScaled(backboardWidth, backboardHeight, 0.1); // Adjust the thickness as needed
    glutSolidCube(1.0);
    glPopMatrix();

    // Draw hoop
    glColor3f(1.0, 0.0, 0.0); // Red color for the hoop
    glPushMatrix();
    glTranslated(0.05, poleHeight + backboardHeight-0.2, 0.15);
    glRotated(90, 1, 0, 0);
    glutSolidTorus(0.02, hoopRadius, 20, 20);
    glPopMatrix();

    glPopMatrix();
}

void updateBasketBallRotation(int value) {
	// Function to update the basketball's rotation
    if (!pauseAnimations) {
		basketBallRotation += 5.0;  // You can adjust the rotation speed as needed

		glutPostRedisplay();
		glutTimerFunc(50, updateBasketBallRotation, 0);
	}
}

void drawSwing(double poleHeight, double poleRadius, double barLength, double barWidth, double seatWidth) {


    glPushMatrix();
    glTranslated(-0.4, -0.4, -0.4);
    glRotated(swingRotation, 0, 1, 0);
    // Draw left pole
    glColor3f(0.5, 0.5, 0.5); // Gray color for the poles
    glPushMatrix();
    glTranslated(-poleRadius / 2, poleHeight / 2, 0);
    glScaled(poleRadius, poleHeight, poleRadius);
    glutSolidCube(1.0);
    glPopMatrix();

    // Draw right pole
    glPushMatrix();
    glTranslated(poleRadius / 2, poleHeight / 2, 0);
    glScaled(poleRadius, poleHeight, poleRadius);
    glutSolidCube(1.0);
    glPopMatrix();

    // Draw horizontal bar (seat support)
    glPushMatrix();
    glColor3f(0.5, 0.2, 0.1); // Brown color for the support bar
    glTranslated(0, poleHeight + barLength / 2, 0);
    glScaled(barWidth, barLength, barWidth);
    glutSolidCube(1.0);
    glPopMatrix();

    // Draw seat using custom drawCylinder function
    glColor3f(1.0, 0.0, 0.0); // Red color for the seat
    glPushMatrix();
    glTranslated(0, poleHeight + barLength, 0);
    glRotated(90, 1, 0, 0); // Rotate the seat to make it horizontal
    drawCylinder(seatWidth / 2, barWidth * 1.5, 20); // Use custom cylinder drawing function
    glPopMatrix();

    glPushMatrix();
    glTranslated(0, poleHeight + barLength - 0.3, 1.5);
    glRotatef(-90, 1, 0, 0); // Rotate the seat to make it horizontal

    // Draw four taller and thinner cylindrical lines under the seat connecting to small cubes
    glColor3f(1, 0, 1); // Dark gray color for the lines
    for (int i = -1; i <= 1; i += 2) {
        glPushMatrix();
        glTranslated(i * (seatWidth / 2), poleHeight + barLength - barWidth * 1.5, 0);
        drawCylinder(barWidth * 1, barWidth * 2, 30); // Adjust the parameters as needed
        glPopMatrix();

        // Draw small cubes at the ends of the lines
        glPushMatrix();
        glTranslated(i * (seatWidth / 2), poleHeight + barLength - barWidth * 1.5, 0);
        glutSolidCube(barWidth * 2); // Adjust the size as needed
        glPopMatrix();
    }

    glPopMatrix();

    glPopMatrix();
}


void updateSwingRotation(int value) {
	// Function to update the swing's rotation
    if (!pauseAnimations) {
		swingRotation += 5.0;  // adjust the rotation speed as needed

		glutPostRedisplay();
		glutTimerFunc(50, updateSwingRotation, 0);
	}
}


void setupLights() {
    GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
    GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
    GLfloat shininess[] = { 50 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
    GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}

void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 640 / 480, 0.001, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.look(Vector3f(playerX, playerY, playerZ));
}


void updateGameTimer(int value) {

        remainingTime--;  // Decrement the remaining time

    if (remainingTime <= 0) {
        // Time is up, set gameLost to true
        isGameOver = true;
    }
        glutPostRedisplay();  // Trigger a redisplay to update the screen

        // Set up the next timer call (e.g., 1000 milliseconds for 1 second)
        glutTimerFunc(1000, updateGameTimer, 0);
}



void print(int x, int y, char* string)
{
    int len, i;

    // Set the position of the text in the window using the x and y coordinates
    glRasterPos2f(x, y);

    // Get the length of the string to display
    len = (int)strlen(string);

    // Loop to display character by character
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    }
}


//void playGameMusic() {
   // gameSound = true;
   // if (gameSound && winSound && loseSound) {
		//PlaySound(TEXT("game_music.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
        // Play the game music
    //PlaySound(TEXT("game_music.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
  //  sndPlaySound(TEXT("Game Sound.wav"), SND_ASYNC | SND_LOOP);
	//}
    /*else if (!gameSound && winSound && !loseSound) {
		PlaySound(TEXT("win_music.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	}
    else if (!gameSound && !winSound && loseSound) {
		PlaySound(TEXT("lose_music.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	}
    else {
		PlaySound(TEXT("game_music.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	}*/
	// Play the game music
	//PlaySound(TEXT("game_music.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
   // sndPlaySound(TEXT("Better call saul - Main theme (8bit version).wav"), SND_ASYNC | SND_LOOP);
//}


/*void playGameMusic() {
    // Play the game music
    if (!gameSound == true && winSound == false && loseSound == false) {
        sndPlaySound(TEXT("Game Sound.wav"), SND_ASYNC | SND_LOOP);
    }
    else if (gameSound == false && winSound == true && loseSound == false) {
        sndPlaySound(TEXT("Win Sound.wav"), SND_ASYNC | SND_LOOP);
	}
	else if (gameSound == false && winSound == false && loseSound == true) {
		sndPlaySound(TEXT("Lose Sound.wav"), SND_ASYNC | SND_LOOP);
	}
	else {
		sndPlaySound(TEXT("Game Sound.wav"), SND_ASYNC | SND_LOOP);

    }
}*/

/*void playWinMusic(bool winSound) {
    
    if (winSound == true) {
		sndPlaySound(TEXT("Win Sound.wav"), SND_ASYNC);
	}

}*/

void backgroundMusic(int x) {
    //std::cout << omarAdel << std::endl;
    if (omarAdel) {
		sndPlaySound(TEXT("Win Sound.wav"), SND_ASYNC);
        glutTimerFunc(6000, backgroundMusic, 0);
	}
	else if (isGameOver) {
		sndPlaySound(TEXT("Lose Sound.wav"), SND_ASYNC);
        glutTimerFunc(13000, backgroundMusic, 0);
	}
    else {
        sndPlaySound(TEXT("Game Sound.wav"), SND_ASYNC);
    glutTimerFunc(14000, backgroundMusic, 0);
    }
}

/*void playGameMusic() {
    if (!gameSound && !winSound && !loseSound) {
        sndPlaySound(TEXT("Game Sound.wav"), SND_ASYNC | SND_LOOP);
    }
    else if (gameSound && winSound && !loseSound) {
        sndPlaySound(TEXT("Win Sound.wav"), SND_ASYNC);
    }
    else if (gameSound && !winSound && loseSound) {
        sndPlaySound(TEXT("Lose Sound.wav"), SND_ASYNC);
    }
 }

void playWinMusic() {
    winSound = true;
    playGameMusic();
}

void playLoseMusic() {
    loseSound = true;
    playGameMusic();
}*/

void Display() {
    setupCamera();
    setupLights();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Check for collision with the balloon
    if (checkGoalCollision()) {
        // Player has collided with the balloon
        // Take appropriate actions, such as incrementing a score, displaying a message, etc.
        // For example: score += 10;

        // Display "Congratulations, You Won!" message
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, 640, 0, 480); // Adjust based on your window dimensions

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glColor3f(0.0, 1, 0.0);

        const char* winMsg = "Congratulations, You Won!";
        glRasterPos2f(640 / 2.0 -120, 480.0 / 2); // Adjust the position as needed

        while (*winMsg) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *winMsg++);
        }
        //sndPlaySound(TEXT("Win Sound.wav"), SND_ASYNC);


        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        //winSound = true;
        //playGameMusic();
        //playWinMusic(winSound);
        //playWinMusic();
        


        // Leave the GLUT main loop to stop further rendering
        //glutLeaveMainLoop();

        //return;
    }
    else if(isGameOver){
        // Display "You Lost!" message
		glPushMatrix();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 640, 0, 480); // Adjust based on your window dimensions

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glColor3f(1, 0.0, 0.0);

        const char* winMsg = "You Lost!";
        glRasterPos2f(640 / 2.0 -120, 480.0 / 2); // Adjust the position as needed

        while (*winMsg) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *winMsg++);
        }

       // playLoseMusic();

        glMatrixMode(GL_PROJECTION);
        glMatrixMode(GL_MODELVIEW);
        
        //loseSound = true;
        //playGameMusic();

		glPopMatrix();
    }
    else {

       // gameSound = true;
       // playGameMusic();

        // Display remaining time
        glPushMatrix();
		glColor3f(0.0, 0.0, 0.0);  // Set color to black
		glRasterPos3f(-0.5, 3.0, -2.0);  // Set position for the text
		char timeMsg[20];
		sprintf(timeMsg, "Time: %d", remainingTime);
		//print(600, 400, timeMsg);
		for (int i = 0; timeMsg[i] != '\0'; ++i) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timeMsg[i]);
		}
        glPopMatrix();

        /*   // Check for collision
           if (checkGoalCollision()) {
               // Collision detected, display "You Won!" message and hide the scene
               //drawText("You Won!", 0.5, 0.5, 1.0, 1.0, 1.0);  // Implement drawText function to display text

                   glColor3d(0, 1, 0); // Green text color

                   char message[] = "Congratulations, you won!";
                   print(450, 300, message);


               // You may also want to reset player position or perform other actions
               return;  // Exit the function to avoid rendering the scene
           }

           glColor3f(0, 0, 0); // Black text color
           char timeMsg[20];
           sprintf(timeMsg, "Time: %d", gameDuration);
           print(20,30 - windowHeight, timeMsg);*/

           /* glPushMatrix();
            glTranslated(0.4, -0.1, 0.6);
            glRotated(45, 0, 0, 1);
            glScaled(0.5, 0.5, 0.5);  // Adjust the scaling factor to make the character larger
            //drawCharacter();
            glPopMatrix();*/
        glPushMatrix();
		glTranslatef(playerX + 0.17, playerY, playerZ);
		//glTranslated(0, -0.2, 0);
		//glTranslatef(0 , 0, 0);
		//glRotatef(playerRotation, 0.0, 0.0, 1.0);
		//printf("%f %f %f\n", playerRotation, playerX, playerZ);
		glRotatef(playerRotation, 0.0, 1.0, 0.0);
        glTranslated(-playerX - 0.17, -playerY, -playerZ);
        
        drawCharacter();
		
        glPopMatrix();


        //drawCharacter();

        glPushMatrix();
        drawGround();
        glPopMatrix();

        // Draw fence
        glPushMatrix();
        drawFence();
        glPopMatrix();

        // Draw Ferris Wheel
        glPushMatrix();
        drawFerrisWheel();
        glPopMatrix();


        // Draw seesaw
        glPushMatrix();
        drawSeeSaw(0.0);
        glPopMatrix();

        // Draw Chinese balloon
        glPushMatrix();
        drawChineseBalloon();
        glPopMatrix();



        glPushMatrix();

        glTranslated(-2, -0.4, 0);

        glPushMatrix();
        glTranslated(0.4, 0.4, 0.6);
        glRotated(45, 0, 0, 1);
        glScaled(0.08, 0.08, 0.08);
        drawJack();
        glPopMatrix();

        glPushMatrix();
        glTranslated(0.6, 0.38, 0.5);
        glRotated(30, 0, 1, 0);
        glutSolidTeapot(0.08);
        glPopMatrix();

        glPushMatrix();
        glTranslated(0.25, 0.42, 0.35);
        glutSolidSphere(0.1, 15, 15);
        glPopMatrix();

        glPushMatrix();
        glTranslated(0.4, 0.0, 0.4);
        drawTable(0.6, 0.02, 0.02, 0.3);
        glRotated(90, 0, 0, 1.0);
        glPopMatrix();

		/*glPushMatrix();
		glRotated(-90, 1.0, 0.0, 0.0);
		glPopMatrix();*/
        glPopMatrix();


        // Draw basketball goal
        glPushMatrix();
        drawBasketballGoal(0.5, 0.1, 0.5, 0.4, 0.1); 
        glPopMatrix();

        // Draw swing
        glPushMatrix();
        drawSwing(1.0, 0.1, 0.5, 0.1, 1.2);
        glPopMatrix();



        // Update camera look based on player position
        camera.look(Vector3f(playerX + 0.24, playerY, playerZ));
        /*   // Draw character
           glPushMatrix();
           glTranslated(0.4, 0.4, 0.6);
           glRotated(45, 0, 0, 1);
           glScaled(0.2, 0.2, 0.2);
           drawCharacter();
           glPopMatrix();*/

    }
        glFlush();
}



void Keyboard(unsigned char key, int x, int y) {
    float d = 0.05;
    float fenceWidth = 0.05;   // Width of each fence picket
    float angle = 1.0;
    float value = 0.94;
    switch (key) {
    case 'w':
        camera.moveY(d);
        break;
    case 's':
        camera.moveY(-d);
        break;
    case 'a':
        camera.moveX(d);
        break;
    case 'd':
        camera.moveX(-d);
        break;
    case 'q':
        camera.moveZ(d);
        break;
    case 'e':
        camera.moveZ(-d);
        break;
    case 'k': // Pause animations
        pauseAnimations = true;
        break;
    case 'l': // Resume animations
        pauseAnimations = false;
        glutTimerFunc(50, updateFerrisWheelRotation, 0);
        glutTimerFunc(50, updateSeeSawMovement, 0);
        glutTimerFunc(50, updateChineseBalloonRotation, 0);
        glutTimerFunc(50, updateJackRotation, 0);
        glutTimerFunc(50, updateBasketBallRotation, 0);
        glutTimerFunc(50, updateSwingRotation, 0);
        break;
    case '8':  // Move forward
        if (playerZ + d < 0.8 - fenceWidth + value && !checkGoalCollision()) {  // Check collision with fence
            if(playerRotation == 0.0)
                playerZ += d;
            playerRotation = 0.0;
        }
        break;
    case '2':  // Move backward
        if (playerZ - d > -1.0 + fenceWidth - value && !checkGoalCollision()) {  // Check collision with fence
			if (playerRotation == 180.0)
                playerZ -= d;
            playerRotation = 180.0;
        }
        break;
    case '6':  // Move right
        if (playerX + d < 0.8 - fenceWidth + value && !checkGoalCollision()) {  // Check collision with fence
			if (playerRotation == 90.0)
                playerX += d;
            //rotatePlayer(-90.0);
            playerRotation = 90.0;
        }
        break;
    case '4':  // Move left
        if (playerX - d > -1.0 + fenceWidth - value - 0.2 && !checkGoalCollision()) {  // Check collision with fence
			if (playerRotation == -90.0)
                playerX -= d;
            playerRotation = -90.0;
        }
        break;
    case '1': // top view
        camera.eye.x = 0.0f;
		camera.eye.y = 6.0f;
		camera.eye.z = 0.0f;
		camera.center.x = 0.0f;
		camera.center.y = 0.0f;
		camera.center.z = 0.0f;
		camera.up.x = 0.0f;
		camera.up.y = -2.0f;
		camera.up.z = -1.0f;
		break;
    case '3': // front view
		camera.eye.x = 0.0f;
        camera.eye.y = 0.0f;
        camera.eye.z = 6.0f;
        camera.center.x = 0.0f;
        camera.center.y = 0.0f;
        camera.center.z = 0.0f;
        camera.up.x = 0.0f;
        camera.up.y = 1.0f;
        camera.up.z = 0.0f;
        break;
    case '5': // side view
        camera.eye.x = 6.0f;
        camera.eye.y = 0.0f;
        camera.eye.z = 0.0f;
        camera.center.x = 0.0f;
        camera.center.y = 0.0f;
        camera.center.z = 0.0f;
        camera.up.x = 0.0f;
        camera.up.y = 1.0f;
        camera.up.z = 0.0f;
        break;
    case '7': // normal view
        camera.eye.x = 1.0f;
		camera.eye.y = 1.0f;
		camera.eye.z = 1.0f;
		camera.center.x = 0.0f;
		camera.center.y = 0.0f;
		camera.center.z = 0.0f;
		camera.up.x = 0.0f;
		camera.up.y = 1.0f;
		camera.up.z = 0.0f;
		break;
    case GLUT_KEY_ESCAPE:
        exit(EXIT_SUCCESS);
    }

    glutPostRedisplay();
}

void Special(int key, int x, int y) {
    float a = 1.0;

    switch (key) {
    case GLUT_KEY_UP:
        camera.rotateX(a);
        break;
    case GLUT_KEY_DOWN:
        camera.rotateX(-a);
        break;
    case GLUT_KEY_LEFT:
        camera.rotateY(a);
        break;
    case GLUT_KEY_RIGHT:
        camera.rotateY(-a);
        break;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitWindowSize(640, 480);
    glutInitWindowPosition(50, 50);

    glutCreateWindow("3moory Land");
    glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(Special);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    // Set up the initial timer call
    glutTimerFunc(50, updateColor, 0);

    // Set up the initial timer call for Ferris Wheel rotation
    glutTimerFunc(500, updateFerrisWheelRotation, 0);

    // Set up the initial timer call for seesaw movement
    glutTimerFunc(50, updateSeeSawMovement, 0);

    // Set up the initial timer call for game timer
    glutTimerFunc(1000, updateGameTimer, 0);

    // Set up the initial timer call for Chinese balloon rotation
    glutTimerFunc(50, updateChineseBalloonRotation, 0);

    // Set up the initial timer call for jack rotation
    glutTimerFunc(50, updateJackRotation, 0);

    // Set up the initial timer call for basketball rotation
    glutTimerFunc(50, updateBasketBallRotation, 0);

    // Set up the initial timer call for swing rotation
    glutTimerFunc(50, updateSwingRotation, 0);

    glutTimerFunc(0, backgroundMusic, 0);


    //playGameMusic();


   /* if (checkGoalCollision()) {
        playWinMusic();
    }
    else if (isGameOver) {
		playLoseMusic();
	}
    else {
		playGameMusic();
	}*/




    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);

    glutMainLoop();

    return 0;
}
