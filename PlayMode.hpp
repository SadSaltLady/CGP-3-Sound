#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

//sounds structures:
struct Notes {
	//musical notes to play on key-press:
	//size = 4
	std::vector< Sound::Sample > samples = {}; // = std::vector< Sound::Sample >(4);
	std::vector< char > correct = {}; // = std::vector< uint8_t >(4);
	//loop
};

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} one, two, three, four;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;
	//Bird object movevments
	struct Bird {
		//Transform parts:
		Scene::Transform *Body = nullptr;
		Scene::Transform *L_wing = nullptr;
		Scene::Transform *R_wing = nullptr;

		//Initial Transform of body
		glm::vec3 init_position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::quat init_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); //n.b. wxyz init order
		//Bird's velocity
		glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	} call1, answer1;


	//movement controller:
	float move_speed = 5.0f;
	void move_bird(Bird &bird, float depth, float height, float dir, float elapsed);

	//hexapod leg to wobble:
	float wobble = 0.0f;

	int current_set = 1;

	//-------music
	struct NotesTracker {
		//current note being played(length 4, null means it's not playing):
		std::vector< std::shared_ptr< Sound::PlayingSample > > samples_playing = // = std::vector< std::shared_ptr< Sound::PlayingSample > >(4);
			std::vector< std::shared_ptr< Sound::PlayingSample > >(4);
		//current loop being played:
		std::shared_ptr< Sound::PlayingSample > loop_playing = nullptr;
		//correct note-- uses 1-4 for the 4 notes, 0 means blank
		std::vector< char > played = std::vector< char >(32, 0);
	} live;
	//glm::vec3 get_leg_tip_position();

	bool recording_response = false;
	//duration of each note:
	float note_duration = 0.4286f /4.f; //really we are counting quaters
	//duration of each loop:
	float loop_duration = 1.7143f*2.f;
	//timer since last loop:
	float loop_timer = 0.f;
	Notes const *activeSet = nullptr;
	bool switch_set(int set);
	//camera:
	Scene::Camera *camera = nullptr;

};
