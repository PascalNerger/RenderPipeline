#pragma once

#include "glm/glm.hpp"

#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/matrix_decompose.hpp"


namespace EngineCore
{

	struct Transform
	{

	public:

		Transform()
			: Rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), Translation(0.0f), Scale3d(1.0f)
		{
		}

		~Transform()
		{

		}

		inline void Translate(const glm::vec3& translation)
		{
			Translation += translation;
		}

		inline void Scale(const glm::vec3& scale)
		{
			Scale3d *= scale;
		}

		inline void Rotate(const glm::vec3& axis, float angle)
		{
			Rotation = glm::rotate(Rotation, glm::radians(angle), axis);
		}

		inline void SetPosition(const glm::vec3& position)
		{
			Translation = position;
		}

		inline void SetScale(const glm::vec3& scale)
		{
			Scale3d = scale;
		}

		inline void SetRotation(const glm::vec3& euler)
		{
			Rotation = glm::quat(glm::radians(euler));
		}

		inline glm::mat4 Inverse() const
		{
			return glm::inverse(GetMatrix());
		}

		inline glm::vec3 GetForward() const
		{
			static glm::vec3 forward = glm::vec3(0, 0, -1);
			return glm::normalize(glm::rotate(Rotation, forward));
		}

		inline glm::vec3 GetBackward() const
		{
			static glm::vec3 backward = glm::vec3(0, 0, 1);
			return glm::normalize(glm::rotate(Rotation, backward));
		}

		inline glm::vec3 GetUp() const
		{
			static glm::vec3 up = glm::vec3(0, 1, 0);
			return glm::normalize(glm::rotate(Rotation, up));
		}

		inline glm::vec3 GetDown() const
		{
			static glm::vec3 down = glm::vec3(0, -1, 0);
			return glm::normalize(glm::rotate(Rotation, down));
		}

		inline glm::vec3 GetLeft() const
		{
			static glm::vec3 left = glm::vec3(-1, 0, 0);
			return glm::normalize(glm::rotate(Rotation, left));
		}

		inline glm::vec3 GetRight() const
		{
			static glm::vec3 right = glm::vec3(1, 0, 0);
			return glm::normalize(glm::rotate(Rotation, right));
		}

		inline float GetPitch() const
		{
			return glm::degrees(glm::pitch(Rotation));
		}

		inline float GetYaw() const
		{
			return glm::degrees(glm::yaw(Rotation));
		}

		inline float GetRoll() const
		{
			return glm::degrees(glm::roll(Rotation));
		}

		inline const glm::vec3& GetPosition() const
		{
			return Translation;
		}

		inline const glm::quat& GetRotation() const
		{
			return Rotation;
		}

		inline const glm::vec3& GetScale() const
		{
			return Scale3d;
		}

		inline glm::vec3 GetEulerAngles() const
		{
			return glm::degrees(glm::eulerAngles(Rotation));
		}

		inline glm::mat4 GetMatrix() const
		{
			glm::mat4 matrix = glm::mat4(1.0f);

			matrix[3][0] = Translation.x;
			matrix[3][1] = Translation.y;
			matrix[3][2] = Translation.z;

			const float x2 = Rotation.x + Rotation.x;
			const float y2 = Rotation.y + Rotation.y;
			const float z2 = Rotation.z + Rotation.z;
			{
				const float xx2 = Rotation.x * x2;
				const float yy2 = Rotation.y * y2;
				const float zz2 = Rotation.z * z2;

				matrix[0][0] = (1.0f - (yy2 + zz2)) * Scale3d.x;
				matrix[1][1] = (1.0f - (xx2 + zz2)) * Scale3d.y;
				matrix[2][2] = (1.0f - (xx2 + yy2)) * Scale3d.z;
			}
			{
				const float yz2 = Rotation.y * z2;
				const float wx2 = Rotation.w * x2;

				matrix[2][1] = (yz2 - wx2) * Scale3d.z;
				matrix[1][2] = (yz2 + wx2) * Scale3d.y;
			}
			{
				const float xy2 = Rotation.x * y2;
				const float wz2 = Rotation.w * z2;

				matrix[1][0] = (xy2 - wz2) * Scale3d.y;
				matrix[0][1] = (xy2 + wz2) * Scale3d.x;
			}
			{
				const float xz2 = Rotation.x * z2;
				const float wy2 = Rotation.w * y2;

				matrix[2][0] = (xz2 + wy2) * Scale3d.z;
				matrix[0][2] = (xz2 - wy2) * Scale3d.x;
			}

			matrix[0][3] = 0.0f;
			matrix[1][3] = 0.0f;
			matrix[2][3] = 0.0f;
			matrix[3][3] = 1.0f;

			return matrix;
		}

		inline glm::mat4 GetMatrixNoScale() const
		{
			glm::mat4 matrix = glm::mat4(1.0f);


			matrix[3][0] = Translation.x;
			matrix[3][1] = Translation.y;
			matrix[3][2] = Translation.z;

			const float x2 = Rotation.x + Rotation.x;
			const float y2 = Rotation.y + Rotation.y;
			const float z2 = Rotation.z + Rotation.z;
			{
				const float xx2 = Rotation.x * x2;
				const float yy2 = Rotation.y * y2;
				const float zz2 = Rotation.z * z2;

				matrix[0][0] = (1.0f - (yy2 + zz2));
				matrix[1][1] = (1.0f - (xx2 + zz2));
				matrix[2][2] = (1.0f - (xx2 + yy2));
			}
			{
				const float yz2 = Rotation.y * z2;
				const float wx2 = Rotation.w * x2;

				matrix[2][1] = (yz2 - wx2);
				matrix[1][2] = (yz2 + wx2);
			}
			{
				const float xy2 = Rotation.x * y2;
				const float wz2 = Rotation.w * z2;

				matrix[1][0] = (xy2 - wz2);
				matrix[0][1] = (xy2 + wz2);
			}
			{
				const float xz2 = Rotation.x * z2;
				const float wy2 = Rotation.w * y2;

				matrix[2][0] = (xz2 + wy2);
				matrix[0][2] = (xz2 - wy2);
			}

			matrix[0][3] = 0.0f;
			matrix[1][3] = 0.0f;
			matrix[2][3] = 0.0f;
			matrix[3][3] = 1.0f;

			return matrix;
		}

		inline glm::mat4 GetMatrixNoRotation() const
		{
			glm::mat4 matrix = glm::mat4(1.0f);

			matrix[3][0] = Translation.x;
			matrix[3][1] = Translation.y;
			matrix[3][2] = Translation.z;

			{
				matrix[0][0] = Scale3d.x;
				matrix[1][1] = Scale3d.y;
				matrix[2][2] = Scale3d.z;
			}

			matrix[0][3] = 0.0f;
			matrix[1][3] = 0.0f;
			matrix[2][3] = 0.0f;
			matrix[3][3] = 1.0f;

			return matrix;
		}

		inline void Reset()
		{
			Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			Translation = glm::vec3(0.0f);
			Scale3d = glm::vec3(1.0f);
		}

	private:

		glm::quat Rotation;
		glm::vec3 Translation;
		glm::vec3 Scale3d;

	};

}