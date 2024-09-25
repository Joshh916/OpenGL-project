#pragma once
#include "common/staticMesh3D.h"

namespace static_meshes_3D {

	/**
	* Cone static mesh with given radius, number of slices and height.
	*/
	class Cone : public StaticMesh3D
	{
	public:
		Cone(float radius, int numSlices, float height,
			bool withPositions = true, bool withTextureCoordinates = true, bool withNormals = true, bool withRotation = false);

		void render() const override;
		void renderPoints() const override;

		/**
		 * Gets Cone radius.
		 */
		float getRadius() const;

		/**
		 * Gets number of Cone slices.
		 */
		int getSlices() const;

		/**
		 * Gets Cone height.
		 */
		float getHeight() const;

	private:
		float _radius; // Cone radius (distance from the center of Cone to surface)
		int _numSlices; // Number of Cone slices
		float _height; // Height of the Cone
		bool _rotation; // boolean to declare the direction of the Cone ie vertical or horizontal
		bool _cup; // boolean to declare if there will be a top

		int _numVerticesSide; // How many vertices to render side of the Cone
		int _numVerticesTopBottom; // How many vertices to render top / bottom of the Cone
		int _numVerticesTotal; // Just a sum of both numbers above

		void initializeData() override;
	};

} // namespace static_meshes_3D