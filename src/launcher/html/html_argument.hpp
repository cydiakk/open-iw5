#pragma once

class html_argument final
{
public:
	html_argument(VARIANT* val);

	bool is_empty() const;

	bool is_string() const;
	bool is_number() const;

	std::string get_string() const;
	int get_number() const;

private:
	VARIANT* value_;
};
