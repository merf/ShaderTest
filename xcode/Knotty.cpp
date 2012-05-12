//
//  Knotty.cpp
//  shaderTest
//
//  Created by Neil Wallace on 12/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "cinder/vector.h"

using namespace ci;

int p = 2;
int q = 3;
float tube_width = 0.2f;
float tube_scale = 1.0f;



Vec3f knot(float t) 
{	
	t *= M_PI * 2.0f;
	Vec3f pt((2 + cos(q * t)) * cos(p * t),
			 (2 + cos(q * t)) * sin(p * t),
			 sin(q * t));
	
	return pt;
}

Vec3f granny_knot(float t) 
{
	t = 2 * M_PI * t;
	
	return Vec3f(-0.22 * cos(t) - 1.28 * sin(t) - 0.44 * cos(3 * t) - 0.78 * sin(3 * t),
				 -0.1 * cos(2 * t) - 0.27 * sin(2 * t) + 0.38 * cos(4 * t) + 0.46 * sin(4 * t),
				 0.7 * cos(3 * t) - 0.4 * sin(3 * t));
}

Vec3f funky(float t)
{
	return knot(t);
}

void draw()
{
	/*
	 const int longitudeSegments = 1000;
	 Vec3f pts[longitudeSegments];
	 Vec3f tangents[longitudeSegments];
	 
	 for(int i=0; i<longitudeSegments; i++)
	 {
	 float t = i/(float)(longitudeSegments);
	 pts[i] = funky(t) * 0.5f;
	 tangents[i] = (funky(t)-funky(t+0.00001f)).normalized();		
	 }
	 
	 Matrix44f m[longitudeSegments];
	 m[0] = firstFrame( pts[0], pts[1], pts[2] );
	 for( int i = 1; i < longitudeSegments - 1; i++ )
	 {
	 m[i] = nextFrame( m[i-1], pts[i-1], pts[i], tangents[i-1], tangents[i] );
	 }
	 m[longitudeSegments-1] = lastFrame( m[longitudeSegments-2], pts[longitudeSegments-2], pts[longitudeSegments-1] );
	 
	 const int latitudeSegments = 50;
	 Vec3f verts[latitudeSegments * longitudeSegments];
	 Vec3f normals[latitudeSegments * longitudeSegments];
	 
	 for(int i=0; i<longitudeSegments; i++)
	 {
	 float df = 1.0f / (float)latitudeSegments;
	 for(int j=0; j<latitudeSegments; ++j)
	 {
	 float a0 = (j * df) * M_PI * 2.0f;
	 
	 normals[j + latitudeSegments * i] = m[i].transformVec(Vec3f(cos(a0), -sin(a0), 0));
	 verts[j +latitudeSegments * i] = m[i].transformPoint(Vec3f(cos(a0) * tube_width, -sin(a0) * tube_width, 0)) * tube_scale;			
	 }
	 }
	 
	 glEnableClientState(GL_VERTEX_ARRAY);	
	 glVertexPointer(3, GL_FLOAT, 0, verts);
	 
	 glEnableClientState(GL_NORMAL_ARRAY);
	 glNormalPointer(GL_FLOAT, 0, normals);
	 
	 
	 GLushort indices[(latitudeSegments+1) * 2];
	 
	 for(int i = 0; i < longitudeSegments-1; i++) 
	 {
	 for(int j = 0; j < latitudeSegments; j++)
	 {
	 indices[j*2+0] = j + latitudeSegments * i;
	 indices[j*2+1] = j + latitudeSegments * (i+1);
	 }
	 indices[latitudeSegments*2+0] = indices[0];
	 indices[latitudeSegments*2+1] = indices[1];
	 glDrawElements( GL_TRIANGLE_STRIP, (latitudeSegments+1)*2, GL_UNSIGNED_SHORT, indices );
	 }
	 
	 //glDrawElements(GL_TRIANGLES, 6*n, GL_UNSIGNED_SHORT, &indices);
	 //	glDrawArrays(GL_TRIANGLES, 0, n * subdivs);
	 
	 glDisableClientState(GL_VERTEX_ARRAY);
	 glDisableClientState(GL_NORMAL_ARRAY);
	 */
}