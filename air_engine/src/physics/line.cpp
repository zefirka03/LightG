#include "line.h"

intersection is_lines_intersect(line2 const& a, line2 const& b){
	glm::mat2 det = glm::mat2({
		(a.b.y - a.a.y), -(a.b.x - a.a.x),
		(b.b.y - b.a.y), -(b.b.x - b.a.x)
	});

	glm::mat2 x1 = glm::mat2({
		a.a.x * (a.b.y - a.a.y) - a.a.y * (a.b.x - a.a.x), -(a.b.x - a.a.x),
		b.a.x * (b.b.y - b.a.y) - b.a.y * (b.b.x - b.a.x), -(b.b.x - b.a.x)
	});

	glm::mat2 z1 = glm::mat2({
		(a.b.y - a.a.y), a.a.x * (a.b.y - a.a.y) - a.a.y * (a.b.x - a.a.x),
		(b.b.y - b.a.y), b.a.x * (b.b.y - b.a.y) - b.a.y * (b.b.x - b.a.x)
	});

    intersection out;
	out.point = { glm::determinant(x1) / glm::determinant(det), glm::determinant(z1) / glm::determinant(det) };
	out.is_intersect = (
		(glm::dot(out.point - a.a, out.point - a.b) < 0) &&
		(glm::dot(out.point - b.a, out.point - b.b) < 0)
	);

	return out;
}