#include "../stdafx.h"
#include "Cue.hpp"

Cue::Cue(std::shared_ptr<CueBallMap> cue_ball_map)
	: Object(Config::cue_path), cue_ball_map_(cue_ball_map)
{
}

void Cue::HandleShot(const std::shared_ptr<Ball>& white_ball, const float dt)
{
	const auto window = glfwGetCurrentContext();
	const auto cue_direction = glm::vec3(sin(angle_), 0.0f, cos(angle_));
	const auto cue_rotation_axis = glm::vec3(cos(angle_) * 0.1f, 1.0f, -sin(angle_) * 0.1f);
	const auto cue_displacement = glm::cross(cue_direction, cue_rotation_axis);

	constexpr auto up = glm::vec3(0.0f, 1.0f, 0.0f);
	const auto power_vector = glm::cross(cue_direction, up);
	auto power = glm::distance(translation_, white_ball->translation_);

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		if (!power_changed_)
		{
			Rotate(cue_rotation_axis, dt);
			Translate(dt * Ball::radius_ * cue_direction);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		if (!power_changed_)
		{
			Rotate(cue_rotation_axis, -dt);
			Translate(-dt * Ball::radius_ * cue_direction);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		if (power > Ball::radius_)
		{
			Translate(-cue_displacement * dt);
			power_changed_ = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		if (power <= 0.5f)
		{
			Translate(cue_displacement * dt);
			power_changed_ = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		power *= Config::power_coeff;
		glm::vec2 spin = cue_ball_map_->GetSpin(); 
		white_ball->Shot(-power_vector * power, spin);
		power_changed_ = false;
	}
}

void Cue::PlaceAtBall(const std::shared_ptr<Ball>& ball)
{
	translation_.x = ball->translation_.x + Ball::radius_ + Config::min_change;
	translation_.y = Ball::radius_;
	translation_.z = ball->translation_.z;

	angle_ = glm::pi<float>();
	rotation_axis_ = glm::vec3(-0.1f, 1.0f, 0.0f);
}

