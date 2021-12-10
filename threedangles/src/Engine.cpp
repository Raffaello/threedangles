#include <Engine.hpp>
#include <cmath>

Mat4 Engine::matrix_pointAt(Vec3d& pos, Vec3d& target, Vec3d& up)
{
    /// @see https://www.3dgep.com/understanding-the-view-matrix/
    Vec3d forward = (target - pos).normalize();

    Vec3d t = (forward * up.dotProd(forward));
    Vec3d newUp = (up - t).normalize();
    Vec3d newRight = newUp.crossProd(forward);

    //Dimensioning & Translation Matrix
    Mat4 m;
    m.m[0][0] = newRight.x; m.m[0][1] = newUp.x; m.m[0][2] = forward.x; m.m[0][3] = pos.x;
    m.m[1][0] = newRight.y; m.m[1][1] = newUp.y; m.m[1][2] = forward.y; m.m[1][3] = pos.y;
    m.m[2][0] = newRight.z; m.m[2][1] = newUp.z; m.m[2][2] = forward.z; m.m[2][3] = pos.z;
    m.m[3][0] = 0.0f;       m.m[3][1] = 0.0f;    m.m[3][2] = 0.0f;      m.m[3][3] = 1.0f;

    return m;
}

Mat4 Engine::matrix_InversePointAt(Mat4& m)
{
    Mat4 matrix;

    matrix.m[0][0] = m.m[0][0];
    matrix.m[1][0] = m.m[0][1];
    matrix.m[2][0] = m.m[0][2];
    matrix.m[3][0] = 0.0f;
    matrix.m[0][1] = m.m[1][0];
    matrix.m[1][1] = m.m[1][1];
    matrix.m[2][1] = m.m[1][2];
    matrix.m[3][1] = 0.0f;
    matrix.m[0][2] = m.m[2][0];
    matrix.m[1][2] = m.m[2][1];
    matrix.m[2][2] = m.m[2][2];
    matrix.m[3][2] = 0.0f;

    matrix.m[0][3] = -(m.m[0][3] * matrix.m[0][0] + m.m[1][3] * matrix.m[0][1] + m.m[2][3] * matrix.m[0][2]);
    matrix.m[1][3] = -(m.m[0][3] * matrix.m[1][0] + m.m[1][3] * matrix.m[1][1] + m.m[2][3] * matrix.m[1][2]);
    matrix.m[2][3] = -(m.m[0][3] * matrix.m[2][0] + m.m[1][3] * matrix.m[2][1] + m.m[2][3] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;

    return matrix;
}




Vec3d Engine::Vector_IntersectPlane(Vec3d& plane_p, Vec3d& plane_n, Vec3d& lineStart, Vec3d& lineEnd)
{
    plane_n = plane_n.normalize();
    float plane_d = -plane_n.dotProd(plane_p);
    float ad = lineStart.dotProd(plane_n);
    float bd = lineEnd.dotProd(plane_n);
    float t = (-plane_d - ad) / (bd - ad);
    Vec3d lineStartToEnd = lineEnd - lineStart; 
    Vec3d lineToIntersect = lineStartToEnd * t;
    return lineStart + lineToIntersect;
}

int Engine::Triangle_ClipAgainstPlane(Vec3d plane_p, Vec3d plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2)
{
    // Make sure plane normal is indeed normal
    plane_n = plane_n.normalize();

    // Return signed shortest distance from point to plane, plane normal must be normalised
    auto dist = [&](Vec3d& p)
    {
        Vec3d n = p.normalize();
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.dotProd(plane_p));
    };

    // Create two temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    Vec3d* inside_points[3]{};  int nInsidePointCount = 0;
    Vec3d* outside_points[3]{}; int nOutsidePointCount = 0;

    // Get signed distance of each point in triangle to plane
    float d0 = dist(in_tri.a);
    float d1 = dist(in_tri.b);
    float d2 = dist(in_tri.c);

    if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.a; }
    else { outside_points[nOutsidePointCount++] = &in_tri.a; }
    if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.b; }
    else { outside_points[nOutsidePointCount++] = &in_tri.b; }
    if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.c; }
    else { outside_points[nOutsidePointCount++] = &in_tri.c; }

    // Now classify triangle points, and break the input triangle into 
    // smaller output triangles if required. There are four possible
    // outcomes...
    if (nInsidePointCount == 0)
    {
        // All points lie on the outside of plane, so clip whole triangle
        // It ceases to exist

        return 0; // No returned triangles are valid
    }

    if (nInsidePointCount == 3)
    {
        // All points lie on the inside of plane, so do nothing
        // and allow the triangle to simply pass through
        out_tri1 = in_tri;

        return 1; // Just the one returned original triangle is valid
    }

    if (nInsidePointCount == 1 && nOutsidePointCount == 2)
    {
        // Triangle should be clipped. As two points lie outside
        // the plane, the triangle simply becomes a smaller triangle

        // Copy appearance info to new triangle
        out_tri1.setColor(in_tri);
        //out_tri1.setColor(64, 0, 0, 255);

        // The inside point is valid, so keep that...
        out_tri1.a = *inside_points[0];

        // but the two new points are at the locations where the 
        // original sides of the triangle (lines) intersect with the plane
        out_tri1.b = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
        out_tri1.c = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

        return 1; // Return the newly formed single triangle
    }

    if (nInsidePointCount == 2 && nOutsidePointCount == 1)
    {
        // Triangle should be clipped. As two points lie inside the plane,
        // the clipped triangle becomes a "quad". Fortunately, we can
        // represent a quad with two new triangles

        // Copy appearance info to new triangles
        out_tri1.setColor(in_tri);
        //out_tri1.setColor(0, 64, 0, 255);
        out_tri2.setColor(in_tri);
        //out_tri2.setColor(0, 0, 64, 255);
        // The first triangle consists of the two inside points and a new
        // point determined by the location where one side of the triangle
        // intersects with the plane
        out_tri1.a = *inside_points[0];
        out_tri1.b = *inside_points[1];
        out_tri1.c = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

        // The second triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the 
        // triangle and the plane, and the newly created point above
        out_tri2.a = *inside_points[1];
        out_tri2.b = out_tri1.c;
        out_tri2.c = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

        return 2; // Return two newly formed triangles which form a quad
    }

    return -1;
}

void Engine::drawTriangle(SDL_Renderer* renderer, const Triangle& triangle)
{
    //compute_int_coord();
    int x1 = static_cast<int>(std::round(triangle.a.x));
    int y1 = static_cast<int>(std::round(triangle.a.y));
    int x2 = static_cast<int>(std::round(triangle.b.x));
    int y2 = static_cast<int>(std::round(triangle.b.y));
    int x3 = static_cast<int>(std::round(triangle.c.x));
    int y3 = static_cast<int>(std::round(triangle.c.y));

    uint8_t r, g, b, a;
    triangle.getColor(r, g, b, a);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLine(renderer, x3, y3, x1, y1);

}

void Engine::fillTriangle(SDL_Renderer* renderer, const Triangle& triangle)
{
    //compute_int_coord();
    int x1 = static_cast<int>(std::round(triangle.a.x));
    int y1 = static_cast<int>(std::round(triangle.a.y));
    int x2 = static_cast<int>(std::round(triangle.b.x));
    int y2 = static_cast<int>(std::round(triangle.b.y));
    int x3 = static_cast<int>(std::round(triangle.c.x));
    int y3 = static_cast<int>(std::round(triangle.c.y));
    
    uint8_t r, g, b, a;
    triangle.getColor(r, g, b, a);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    // at first sort the three vertices by y-coordinate ascending so v1 is the topmost vertice
    if (y1 > y2) {
        std::swap(y1, y2);
        std::swap(x1, x2);
    }
    if (y1 > y3) {
        std::swap(y1, y3);
        std::swap(x1, x3);
    }
    if (y2 > y3) {
        std::swap(y2, y3);
        std::swap(x2, x3);
    }

    int t1x = 0, t2x = 0, y = 0, minx = 0, maxx = 0, t1xp = 0, t2xp = 0;
    bool changed1 = false;
    bool changed2 = false;

    int8_t signx1 = 0, signx2 = 0;

    int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
    int e1 = 0, e2 = 0;

    // -----

    t1x = t2x = x1;
    y = y1;
    changed1 = changed2 = false;

    //fill_top_flat(renderer);
    dy1 = y2 - y1;
    dx1 = x2 - x1;
    if (dx1 < 0) {
        dx1 = -dx1;
        signx1 = -1;
    }
    else {
        signx1 = 1;
    }

    dy2 = y3 - y1;
    dx2 = x3 - x1;
    if (dx2 < 0) {
        dx2 = -dx2;
        signx2 = -1;
    }
    else
        signx2 = 1;

    // swap values
    if (dy1 > dx1) {
        std::swap(dx1, dy1);
        changed1 = true;
    }
    if (dy2 > dx2) {
        std::swap(dy2, dx2);
        changed2 = true;
    }

    /* here we know that v1.y <= v2.y <= v3.y */
    /* check for trivial case of bottom-flat triangle */

    e2 = dx2 >> 1;
    // Flat top, just process the second half
    if (y1 == y2)
        goto next;

    e1 = dx1 >> 1;
    for (int i = 0; i < dx1;)
    {
        t1xp = 0;
        t2xp = 0;
        if (t1x < t2x)
        {
            minx = t1x;
            maxx = t2x;
        }
        else {
            minx = t2x;
            maxx = t1x;
        }

        // process first line until y value is about to change
        while (i < dx1)
        {
            i++;
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1)
                    t1xp = signx1;
                else
                    goto next1;
            }

            if (changed1)
                break;
            else
                t1x += signx1;
        }
        // Move line
    next1:
        // process second line until y value is about to change
        while (true)
        {
            e2 += dy2;
            while (e2 >= dx2)
            {
                e2 -= dx2;
                if (changed2)
                    t2xp = signx2;
                else
                    goto next2;
            }
            if (changed2)
                break;
            else
                t2x += signx2;
        }
    next2:
        //fill_update_minmax();
        if (minx > t1x)
            minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;
        // Draw line from min to max points found on the y
        draw_hline(renderer, minx, maxx, y);
        // Now increase y
        if (!changed1)
            t1x += signx1;
        t1x += t1xp;
        if (!changed2)
            t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2)
            break;
    }
next:
    // Second half
    //fill_bottom_flat(renderer);
    dy1 = y3 - y2;
    dx1 = x3 - x2;
    if (dx1 < 0) {
        dx1 = -dx1;
        signx1 = -1;
    }
    else
        signx1 = 1;

    t1x = x2;
    if (dy1 > dx1)
    {   // swap values
        std::swap(dy1, dx1);
        changed1 = true;
    }
    else
        changed1 = false;

    e1 = dx1 >> 1;
    for (int i = 0; i <= dx1; i++)
    {
        t1xp = 0; t2xp = 0;
        if (t1x < t2x)
        {
            minx = t1x;
            maxx = t2x;
        }
        else
        {
            minx = t2x;
            maxx = t1x;
        }
        // process first line until y value is about to change
        while (i < dx1)
        {
            e1 += dy1;
            while (e1 >= dx1)
                //if (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1)
                {
                    t1xp = signx1;
                    //break;
                }//t1x += signx1;
                else
                    goto next3;
            }
            if (changed1)
                break;
            else
                t1x += signx1;
            if (i < dx1)
                i++;
        }
    next3:
        // process second line until y value is about to change
        while (t2x != x3)
        {
            e2 += dy2;
            while (e2 >= dx2)
            {
                e2 -= dx2;
                if (changed2)
                    t2xp = signx2;
                else
                    goto next4;
            }

            if (changed2)
                break;
            else
                t2x += signx2;
        }
    next4:
        //fill_update_minmax();
        if (minx > t1x)
        minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;
        // Draw line from min to max points found on the y
        draw_hline(renderer, minx, maxx, y);
        // Now increase y
        if (!changed1)
            t1x += signx1;
        t1x += t1xp;
        if (!changed2)
            t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y > y3)
            break;
    }
}

void Engine::draw_hline(SDL_Renderer* renderer, int x1, int x2, const int y) noexcept
{
    if (x1 >= x2) std::swap(x1, x2);
    for (; x1 <= x2; x1++) {
        SDL_RenderDrawPoint(renderer, x1, y);
    }
}
