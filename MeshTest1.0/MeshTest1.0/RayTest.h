#ifndef RAYTEST_H
#define RAYTEST_H

//===================把屏幕坐标转化为游戏世界三维坐标==============================//
void ScreenPosToWorldRay(
	int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
	int screenWidth, int screenHeight,  // Window size, in pixels
	glm::mat4& ViewMatrix,               // Camera position and orientation 摄像机的view矩阵
	glm::mat4& ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes) 摄像机的投影矩阵
	glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
	glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
	)
{
	glm::vec4 lRayStart_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f, // [0,640] -> [-1,1]
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f, // [0, 480] -> [-1,1]
		-1.0f, // The near plane maps to Z=-1 in Normalized Device Coordinates
		1.0f
		);
	glm::vec4 lRayEnd_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
		1.0f,
		1.0f
		);

	// Faster way (just one inverse)
	glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
	glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world /= lRayStart_world.w;
	glm::vec4 lRayEnd_world = M * lRayEnd_NDC; lRayEnd_world /= lRayEnd_world.w;


	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = glm::normalize(lRayDir_world);

	// Output
	out_origin = glm::vec3(lRayStart_world);
	out_direction = glm::normalize(lRayDir_world);
}

//============================检测射线碰撞事件===================================//
bool TestRayOBBIntersection(
	glm::vec3 ray_origin,        // Ray origin, in world space
	glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
	glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
	glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
	glm::mat4& modelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
	float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
	)
{
	float tMin = 0.0f;
	float tMax = 100000.0f;

	glm::vec3 OBBposition_worldspace(modelMatrix[3].x, modelMatrix[3].y, modelMatrix[3].z);

	glm::vec3 delta = OBBposition_worldspace - ray_origin;	// from Ray Origin to OBB position

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		glm::vec3 xaxis(modelMatrix[0].x, modelMatrix[0].y, modelMatrix[0].z);
		float e = glm::dot(xaxis, delta);
		float f = glm::dot(ray_direction, xaxis);

		if (fabs(f) > 0.001f) { // Standard case

			float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
			float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
			if (t1>t2) {
				float w = t1; t1 = t2; t2 = w; // swap t1 and t2
			}

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;

			if (tMax < tMin)
				return false;
		}
		else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
		glm::vec3 yaxis(modelMatrix[1].x, modelMatrix[1].y, modelMatrix[1].z);
		float e = glm::dot(yaxis, delta);
		float f = glm::dot(ray_direction, yaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.y) / f;
			float t2 = (e + aabb_max.y) / f;

			if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;
		}
		else {
			if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
		glm::vec3 zaxis(modelMatrix[2].x, modelMatrix[2].y, modelMatrix[2].z);
		float e = glm::dot(zaxis, delta);
		float f = glm::dot(ray_direction, zaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.z) / f;
			float t2 = (e + aabb_max.z) / f;

			if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;
		}
		else {
			if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
				return false;
		}
	}

	intersection_distance = tMin;
	return true;

}

#endif 