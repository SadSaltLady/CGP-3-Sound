#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint hexapod_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > hexapod_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("forest.pnct"));
	hexapod_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > hexapod_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("forest.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = hexapod_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = hexapod_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Load< Notes > Set1(LoadTagDefault, []() -> Notes const * {
	Notes *ret = new Notes();
	//load sound sample set1:

	ret->samples.push_back(Sound::Sample(data_path("call1-test.wav")));
	ret->samples.push_back(Sound::Sample(data_path("answer1-1.opus")));
	ret->samples.push_back(Sound::Sample(data_path("answer1-2.opus")));
	ret->samples.push_back(Sound::Sample(data_path("answer1-3.opus")));
	ret->samples.push_back(Sound::Sample(data_path("answer1-4.opus")));

	ret->correct = {0,0,0,0,
					0,0,0,0,
					1,1,1,1,
					0,0,0,0, //-----------------16
					0,0,0,0,
					0,0,0,0,
					1,1,1,1,
					0,0,0,0, //-----------------32
					};
	//returns dummy
	return ret;	
});

Load< Notes > Set2(LoadTagDefault, []() -> Notes const * {
	Notes *ret = new Notes();
	//load sound sample set1:

	ret->samples.push_back(Sound::Sample(data_path("call2.opus")));
	ret->samples.push_back(Sound::Sample(data_path("answer2-1.opus")));
	ret->samples.push_back(Sound::Sample(data_path("answer2-2.opus")));
	ret->samples.push_back(Sound::Sample(data_path("answer2-3.opus")));
	ret->samples.push_back(Sound::Sample(data_path("answer2-4.opus")));

	ret->correct = {1,1,0,0,
					1,1,0,0,
					1,1,0,0,
					1,1,0,0, //-----------------16
					2,2,0,0,
					2,2,0,0,
					2,2,0,0,
					2,2,0,0, //-----------------32
					};
	//returns dummy
	return ret;
});

Load< Notes > Set3(LoadTagDefault, []() -> Notes const * {
	Notes *ret = new Notes();
	//load sound sample set1:

	ret->samples.push_back(Sound::Sample(data_path("call3.opus")));
	ret->samples.push_back(Sound::Sample(data_path("answer3-1.opus")));
	ret->samples.push_back(Sound::Sample(data_path("answer3-2.opus")));
	ret->samples.push_back(Sound::Sample(data_path("answer3-3.opus")));
	ret->samples.push_back(Sound::Sample(data_path("answer3-4.opus")));

	ret->correct = {1,1,1,1,
					1,1,1,1,
					3,3,3,3,
					2,2,2,2, //-----------------16
					3,3,3,3,
					3,3,3,3,
					1,1,1,1,
					0,0,0,0, //-----------------32
					};
	//returns dummy
	return ret;
});

Load< Notes > Set4(LoadTagDefault, []() -> Notes const * {
	Notes *ret = new Notes();
	//load sound sample set1:

	ret->samples.push_back(Sound::Sample(data_path("final.opus")));
	//returns dummy
	ret->correct = {0,0,0,0,
				0,0,0,0,
				0,0,0,0,
				0,0,0,0, //-----------------16
				0,0,0,0,
				0,0,0,0,
				0,0,0,0,
				0,0,0,0, //-----------------32
				};
	return ret;
});

PlayMode::PlayMode() : scene(*hexapod_scene) {
	std::cout << "did it reach here\n";
	//get pointers to leg for convenience:
	/*
	for (auto &transform : scene.transforms) {
		if (transform.name == "Hip.FL") hip = &transform;
		else if (transform.name == "UpperLeg.FL") upper_leg = &transform;
		else if (transform.name == "LowerLeg.FL") lower_leg = &transform;
	}
	if (hip == nullptr) throw std::runtime_error("Hip not found.");
	if (upper_leg == nullptr) throw std::runtime_error("Upper leg not found.");
	if (lower_leg == nullptr) throw std::runtime_error("Lower leg not found.");

	hip_base_rotation = hip->rotation;
	upper_leg_base_rotation = upper_leg->rotation;
	lower_leg_base_rotation = lower_leg->rotation;
	*/
	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	//start music loop playing:
	// (note: position will be over-ridden in update())
	
	live.loop_playing = Sound::loop_3D(Set1->samples[0], 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), 10.0f);
	activeSet = Set1;
	std::cout << "what about here\n";
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			one.downs += 1;
			one.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			two.downs += 1;
			two.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			three.downs += 1;
			three.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			four.downs += 1;
			four.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			one.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			two.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			three.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			four.pressed = false;
			return true;
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	/*
	wobble += elapsed / 10.0f;
	wobble -= std::floor(wobble);

	hip->rotation = hip_base_rotation * glm::angleAxis(
		glm::radians(5.0f * std::sin(wobble * 2.0f * float(M_PI))),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	upper_leg->rotation = upper_leg_base_rotation * glm::angleAxis(
		glm::radians(7.0f * std::sin(wobble * 2.0f * 2.0f * float(M_PI))),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);
	lower_leg->rotation = lower_leg_base_rotation * glm::angleAxis(
		glm::radians(10.0f * std::sin(wobble * 3.0f * 2.0f * float(M_PI))),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);
	*/
	//update timer:
	loop_timer += elapsed;
	bool switched = false;
	if (loop_timer > loop_duration) { //end of one loop
		//check if we are correct
		if (recording_response) {
			int score = 0;
			for (int i = 0; i < 32; i++) {
				if (live.played[i] == activeSet->correct[i]) {
					score++;
				}
			}

			if (score > 25) {
				std::cout << "You got " << score << " out of 32 correct. You passed!\n";
				current_set++;
				switch_set(current_set);
				loop_timer = 0.0f;
				switched = true;
			}
			else {
				std::cout << "You got " << score << " out of 32 correct. Try again.\n";
			}
			std::string s(live.played.begin(), live.played.end());
			std::string t(activeSet->correct.begin(), activeSet->correct.end());
			std::cout << "Played: " << s << "\n";
			std::cout << "Corect: " << t << "\n";
			std::cout << "Score: " << score << "\n";
		}

		if (!switched) {
			recording_response = !recording_response;
			live.played = std::vector< char >(32, 0);
			loop_timer -= loop_duration;
		} else {
			recording_response = false;
			loop_timer = -0.01f;
			live.played = std::vector< char >(32, 0);
		}
	}
	//check if whatever we are playing is correct:
	int idx = 0;
	if (recording_response) {
		idx = (int)floor(loop_timer / note_duration);
		//std::cout << "idx: " << idx << "\n";
	}
	
	

	//chirp:
	//if key pressed, play chirp; when released, stop chirp
	//if not playing, then play chirp
	if (current_set < 4) {

		
		if (one.pressed) {
			live.played[idx] = 1;
			if (live.samples_playing[0] == nullptr) {
				live.samples_playing[0] = Sound::play_3D(activeSet->samples[1], 10.0f, glm::vec3(0.0f, 0.0f, 0.0f), 10.0f);
			}
		} else if (!one.pressed && live.samples_playing[0] != nullptr) {
			//stop chirp:
			live.samples_playing[0]->stop();
			live.samples_playing[0] = nullptr;
		}

		if (two.pressed) {
			live.played[idx] = 2;
			if (live.samples_playing[1] == nullptr) {
				live.samples_playing[1] = Sound::play_3D(activeSet->samples[2], 10.0f, glm::vec3(0.0f, 0.0f, 0.0f), 10.0f);
			}
		} else if (!two.pressed && live.samples_playing[1] != nullptr) {
			//stop chirp:
			live.samples_playing[1]->stop();
			live.samples_playing[1] = nullptr;
		}

		if (three.pressed) {
			live.played[idx] = 3;
			if (live.samples_playing[2] == nullptr) {
				live.samples_playing[2] = Sound::play_3D(activeSet->samples[3], 10.0f, glm::vec3(0.0f, 0.0f, 0.0f), 10.0f);
			}
		} else if (!three.pressed && live.samples_playing[2] != nullptr) {
			//stop chirp:
			live.samples_playing[2]->stop();
			live.samples_playing[2] = nullptr;
		}

		if (four.pressed) {
			live.played[idx] = 4;
			if (live.samples_playing[3] == nullptr) {
				live.samples_playing[3] = Sound::play_3D(activeSet->samples[4], 10.0f, glm::vec3(0.0f, 0.0f, 0.0f), 10.0f);
			}
		} else if (!four.pressed && live.samples_playing[3] != nullptr) {
			//stop chirp:
			live.samples_playing[3]->stop();
			live.samples_playing[3] = nullptr;
		}
	}



	{ //update listener to camera position:
		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 frame_right = frame[0];
		glm::vec3 frame_at = frame[3];
		Sound::listener.set_position_right(frame_at, frame_right, 1.0f / 60.0f);
	}

	//reset button press counters:
	one.downs = 0;
	two.downs = 0;
	three.downs = 0;
	four.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		/*
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
		*/
	}
	GL_ERRORS();
}

/*
glm::vec3 PlayMode::get_leg_tip_position() {
	//the vertex position here was read from the model in blender:
	return lower_leg->make_local_to_world() * glm::vec4(-1.26137f, -11.861f, 0.0f, 1.0f);
}
*/

void PlayMode::move_bird(Bird &bird, float depth, float height, float dir, float elapsed) 
{
	//transform screen space values to object space:
	//only look at dir for now -> camera is pointing down the X axis thus +y is right, and -y is left
	glm::vec3 dir_vec = glm::vec3(0.0f, dir, 0.0f);
	glm::vec3 dir_vec_world = camera->transform->make_local_to_world() * glm::vec4(dir_vec, 1.0f);
	glm::vec3 dir_vec_world_norm = glm::normalize(dir_vec_world);
	//apply to bird transform
	bird.Body->position += dir_vec_world_norm * elapsed * move_speed;
}

bool PlayMode::switch_set(int set)
{
	if (set > 4) {
		return false;
	}
	if (set == 2) {
		activeSet = Set2;
	} else if (set == 3) {
		activeSet = Set3;
	} else if (set == 4) {
		activeSet = Set4;
	}
	//stop current loop
	if (live.loop_playing != nullptr) {
		live.loop_playing->stop();
		live.loop_playing = nullptr;
		live.loop_playing = Sound::loop_3D(activeSet->samples[0], 10.0f, glm::vec3(0.0f, 0.0f, 0.0f), 10.0f);
	}
	return true;
}