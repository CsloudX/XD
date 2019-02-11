#ifndef X_TRANSFORM_HPP
#define X_TRANSFORM_HPP

#include "XVector.hpp"

class XTransform: public XMatrix4x4
{
public:
	XTransform() { reset(); }
	XTransform(const XMatrix& other)
	{
		assert(other.rowCount() == rowCount() && other.columnCount() == columnCount());
		m_data = other.data();
	}
	void reset()
	{
		foreachValueRef([](int row, int col, double& value) {
			value = (row == col) ? 1 : 0;
		});
	}

	void transform(const XMatrix4x4& mat) { (*this) = (*this)*mat; }

	void move(double x, double y, double z) { transform(moveMatrix(x, y, z)); }
	void move(const XVector3& vec) { move(vec.x(), vec.y(), vec.z()); }
	void rotate(double xDeg, double yDeg, double zDeg) { transform(rotateMatrix(xDeg, yDeg, zDeg)); }
	void rotate(const XVector3& vec) { rotate(vec.x(), vec.y(), vec.z()); }
	void scale(double kx, double ky, double kz) { transform(scaleMatrix(kx, ky, kz)); }
	void scale(const XVector3& vec) { scale(vec.x(), vec.y(), vec.z()); }
	void project(double x, double y, double z) { transform(projectMatrix(x, y, z)); }

	static XMatrix4x4 moveMatrix(double x, double y, double z)
	{
		XTransform mat;
		mat(3, 0) = x;
		mat(3, 1) = y;
		mat(3, 2) = z;
		return mat;
	}
	static XMatrix4x4 projectMatrix(double x, double y, double z)
	{
		return XMatrix4x4(
			1 - x*x, -x*y, -x*z, 0,
			-x*y, 1 - y*y, -y*z, 0,
			-x*z, -y*z, 1 - z*z, 0,
			0, 0, 0, 1
		);
	}
	static XMatrix4x4 rotateMatrix(double xDeg, double yDeg, double zDeg)
	{
		XTransform ret;
		if (xDeg != 0) {
			double rad = XMath::deg2rad(xDeg);
			double sinVal = XMath::sin(rad);
			double cosVal = XMath::cos(rad);
			XMatrix4x4 mat(
				1,0,0,0,
				0,cosVal,sinVal,0,
				0,-sinVal,cosVal,0,
				0,0,0,1
			);
			ret = ret*mat;
		}
		if (yDeg != 0) {
			double rad = XMath::deg2rad(yDeg);
			double sinVal = XMath::sin(rad);
			double cosVal = XMath::cos(rad);
			XMatrix4x4 mat(
				cosVal,0,-sinVal,0,
				0,1,0,0,
				sinVal,0,cosVal,0,
				0,0,0,1
			);
			ret = ret*mat;
		}
		if (zDeg != 0) {
			double rad = XMath::deg2rad(zDeg);
			double sinVal = XMath::sin(rad);
			double cosVal = XMath::cos(rad);
			XMatrix4x4 mat(
				cosVal,sinVal,0,0,
				-sinVal,cosVal,0,0,
				0,0,1,0,
				0,0,0,1
			);
			ret = ret*mat;
		}
		return ret;
	}

	static XMatrix4x4 scaleMatrix(double kx, double ky, double kz)
	{
		return XMatrix4x4(
			kx,0,0,0,
			0,ky,0,0,
			0,0,kz,0,
			0,0,0,1
		);
	}

	static XMatrix2x2 rotationMatrix(double deg)
	{
		double rad = XMath::deg2rad(deg);
		double sinVal = XMath::sin(rad);
		double cosVal = XMath::cos(rad);
		return XMatrix2x2(cosVal,sinVal,-sinVal,cosVal );
	}

	static XMatrix3x3 rotationMatrix(double xDeg, double yDeg, double zDeg)
	{
		XMatrix3x3 ret = XMatrix::identityMatrix(3);
		if (xDeg != 0) {
			double rad = XMath::deg2rad(xDeg);
			double sinVal = XMath::sin(rad);
			double cosVal = XMath::cos(rad);
			XMatrix mat(3, 3, {
				1,0,0,
				0,cosVal,sinVal,
				0,-sinVal,cosVal
			});
			ret = ret*mat;
		}
		if (yDeg != 0) {
			double rad = XMath::deg2rad(yDeg);
			double sinVal = XMath::sin(rad);
			double cosVal = XMath::cos(rad);
			XMatrix mat(3, 3, {
				cosVal,0,-sinVal,
				0,1,0,
				sinVal,0,cosVal
			});
			ret = ret*mat;
		}
		if (zDeg != 0) {
			double rad = XMath::deg2rad(zDeg);
			double sinVal = XMath::sin(rad);
			double cosVal = XMath::cos(rad);
			XMatrix mat(3, 3, {
				cosVal,sinVal,0,
				-sinVal,cosVal,0,
				0,0,1
			});
			ret = ret*mat;
		}
		return ret;
	}

	static XMatrix3x3 rotationMatrix(const XVector3& vec, double deg)
	{
		XVector3 n = vec.normalized();
		double rad = XMath::deg2rad(deg);
		double sinVal = XMath::sin(rad);
		double cosVal = XMath::cos(rad);
		double a = 1.0 - cosVal;
		return XMatrix3x3(
			n.x()*n.x()*a + cosVal, n.x()*n.y()*a + n.z()*sinVal, n.x()*n.z()*a - n.y()*sinVal,
			n.x()*n.y()*a + n.z()*sinVal, n.y()*n.y()*a + cosVal, n.y()*n.z()*a + n.x()*sinVal,
			n.x()*n.z()*a + n.y()*sinVal, n.y()*n.z()*a - n.x()*sinVal, n.z()*n.z()*a + cosVal
		);
	}



	static XMatrix2x2 projected(const XVector2& vec)
	{
		XVector2 n = vec.normalized();
		return XMatrix2x2 (1 - n.x()*n.x(),-n.x()*n.y(),-n.x()*n.y(),1 - n.y()*n.y() );
	}

	static XMatrix3x3 projected(const XVector3& vec)
	{
		XVector3 n = vec.normalized();
		return XMatrix3x3 (
			1-n.x()*n.x(),-n.x()*n.y(),-n.x()*n.z(),
			-n.x()*n.y(),1-n.y()*n.y(),-n.y()*n.z(),
			-n.x()*n.z(),-n.y()*n.z(),1-n.z()*n.z()
		);
	}
};

#endif // XTRANSFORM_HPP
