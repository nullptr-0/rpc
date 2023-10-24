#pragma once

class MyType {
protected:
	int someProtectedOrPrivateField;
public:
	int first;
	int second;
	
	// Note that to avoid unnecessary complexity, it's strongly recommanded that custom types have a default constructor¡£
	MyType() : first(0), second(0), someProtectedOrPrivateField(0) {}
	
	MyType(int v1, int v2) : first(v1), second(v2), someProtectedOrPrivateField(0) {}
	int get() const {
		return someProtectedOrPrivateField;
	}
	void set(const int val) {
		someProtectedOrPrivateField = val;
	}
};