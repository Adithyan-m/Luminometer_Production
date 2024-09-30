#include <stdio.h>
#include <math.h>
#include <string.h>

#define NUM_POINTS 10
#define PLOT_WIDTH 60
#define PLOT_HEIGHT 20

typedef struct {
    float x;
    float y;
} Point;

// Updated function prototypes
void pivotCurve(Point* curve, Point pivotPoint, float angle);
void shiftCurve(Point* curve, float dx, float dy);
void fitCurve(Point* curve, Point newPoint1, Point newPoint2);
float interpolateY(Point* curve, float x);
void plotCurves(Point* curve1, Point* curve2, int numPoints);
float calculateAngle(Point p1, Point p2);

int main() {
    // Define the reference curve (10 points)
    Point referenceCurve[NUM_POINTS] = {
        {0, 0}, {1, 2}, {2, 8}, {3, 18}, {4, 32},
        {5, 50}, {6, 72}, {7, 98}, {8, 128}, {9, 162}
    };

    // Define the two new points
    Point newPoint1 = {2, 5};
    Point newPoint2 = {10, 200};

    // Perform the curve fitting
    fitCurve(referenceCurve, newPoint1, newPoint2);

    // Print the results
    printf("Fitted Curve:\n");
    for (int i = 0; i < NUM_POINTS; i++) {
        printf("Point %d: (%.2f, %.2f)\n", i, referenceCurve[i].x, referenceCurve[i].y);
    }

    // Test interpolation
    float testX = 5.5;
    float interpolatedY = interpolateY(referenceCurve, testX);
    printf("Interpolated Y for X=%.2f: %.2f\n", testX, interpolatedY);

    // Plot the original and fitted curves
    printf("\nCurve Visualization:\n");
    printf("Original: *  Fitted: +\n");
    plotCurves(referenceCurve, referenceCurve, NUM_POINTS);  // Note: We're using referenceCurve twice here as it's modified in place

    return 0;
}

// The rest of the function implementations remain the same

void pivotCurve(Point* curve, Point pivotPoint, float angle) {
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    for (int i = 0; i < NUM_POINTS; i++) {
        float dx = curve[i].x - pivotPoint.x;
        float dy = curve[i].y - pivotPoint.y;

        curve[i].x = pivotPoint.x + dx * cosAngle - dy * sinAngle;
        curve[i].y = pivotPoint.y + dx * sinAngle + dy * cosAngle;
    }
}

void shiftCurve(Point* curve, float dx, float dy) {
    for (int i = 0; i < NUM_POINTS; i++) {
        curve[i].x += dx;
        curve[i].y += dy;
    }
}

float calculateAngle(Point p1, Point p2) {
    return atan2f(p2.y - p1.y, p2.x - p1.x);
}

void fitCurve(Point* curve, Point newPoint1, Point newPoint2) {
    // Find the two points in the original curve that are closest to the x-coordinates of newPoint1 and newPoint2
    int idx1 = (int)(newPoint1.x + 0.5f);
    int idx2 = (int)(newPoint2.x + 0.5f);
    
    if (idx1 < 0) idx1 = 0;
    if (idx1 >= NUM_POINTS) idx1 = NUM_POINTS - 1;
    if (idx2 < 0) idx2 = 0;
    if (idx2 >= NUM_POINTS) idx2 = NUM_POINTS - 1;

    // Calculate the angle between the original points and the new points
    float originalAngle = calculateAngle(curve[idx1], curve[idx2]);
    float newAngle = calculateAngle(newPoint1, newPoint2);
    float rotationAngle = newAngle - originalAngle;

    // Pivot the curve around the first point
    pivotCurve(curve, curve[idx1], rotationAngle);

    // Calculate and apply the shift
    float dx = newPoint1.x - curve[idx1].x;
    float dy = newPoint1.y - curve[idx1].y;
    shiftCurve(curve, dx, dy);
}

float interpolateY(Point* curve, float x) {
    // Find the two points that bracket the given x
    int i;
    for (i = 0; i < NUM_POINTS - 1; i++) {
        if (curve[i].x <= x && x <= curve[i+1].x) {
            break;
        }
    }

    // If x is out of range, return the y-value of the closest endpoint
    if (i == NUM_POINTS - 1) {
        return (x < curve[0].x) ? curve[0].y : curve[NUM_POINTS-1].y;
    }

    // Perform linear interpolation
    float t = (x - curve[i].x) / (curve[i+1].x - curve[i].x);
    return curve[i].y + t * (curve[i+1].y - curve[i].y);
}
void plotCurves(Point* curve1, Point* curve2, int numPoints) {
    char plot[PLOT_HEIGHT][PLOT_WIDTH];
    memset(plot, ' ', sizeof(plot));

    // Find min and max values
    float minX = curve1[0].x, maxX = curve1[0].x;
    float minY = curve1[0].y, maxY = curve1[0].y;
    for (int i = 0; i < numPoints; i++) {
        if (curve1[i].x < minX) minX = curve1[i].x;
        if (curve1[i].x > maxX) maxX = curve1[i].x;
        if (curve1[i].y < minY) minY = curve1[i].y;
        if (curve1[i].y > maxY) maxY = curve1[i].y;
        if (curve2[i].x < minX) minX = curve2[i].x;
        if (curve2[i].x > maxX) maxX = curve2[i].x;
        if (curve2[i].y < minY) minY = curve2[i].y;
        if (curve2[i].y > maxY) maxY = curve2[i].y;
    }

    // Plot the curves
    for (int i = 0; i < numPoints; i++) {
        int x1 = (int)((curve1[i].x - minX) / (maxX - minX) * (PLOT_WIDTH - 1));
        int y1 = (int)((curve1[i].y - minY) / (maxY - minY) * (PLOT_HEIGHT - 1));
        int x2 = (int)((curve2[i].x - minX) / (maxX - minX) * (PLOT_WIDTH - 1));
        int y2 = (int)((curve2[i].y - minY) / (maxY - minY) * (PLOT_HEIGHT - 1));

        y1 = PLOT_HEIGHT - 1 - y1;  // Invert y-axis
        y2 = PLOT_HEIGHT - 1 - y2;  // Invert y-axis

        if (x1 >= 0 && x1 < PLOT_WIDTH && y1 >= 0 && y1 < PLOT_HEIGHT)
            plot[y1][x1] = '*';
        if (x2 >= 0 && x2 < PLOT_WIDTH && y2 >= 0 && y2 < PLOT_HEIGHT)
            plot[y2][x2] = '+';
    }

    // Print the plot
    for (int i = 0; i < PLOT_HEIGHT; i++) {
        for (int j = 0; j < PLOT_WIDTH; j++) {
            putchar(plot[i][j]);
        }
        putchar('\n');
    }
}