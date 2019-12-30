//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     DesignSystem data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

#include <optional>
#include <stdexcept>
#include <regex>

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann
{
    template<typename T>
    struct adl_serializer<std::shared_ptr<T>>
    {
        static void to_json(json &j, const std::shared_ptr<T> &opt)
        {
            if (!opt)
                j = nullptr;
            else
                j = *opt;
        }

        static std::shared_ptr<T> from_json(const json &j)
        {
            if (j.is_null())
                return std::unique_ptr<T>();
            else
                return std::unique_ptr<T>(new T(j.get<T>()));
        }
    };
}
#endif

namespace Youi
{
    using nlohmann::json;

    class ClassMemberConstraints
    {
    private:
        std::optional<int> min_value;
        std::optional<int> max_value;
        std::optional<size_t> min_length;
        std::optional<size_t> max_length;
        std::optional<std::string> pattern;

    public:
        ClassMemberConstraints(
            std::optional<int> min_value,
            std::optional<int> max_value,
            std::optional<size_t> min_length,
            std::optional<size_t> max_length,
            std::optional<std::string> pattern
        ) : min_value(min_value), max_value(max_value), min_length(min_length), max_length(max_length), pattern(pattern)
        {
        }

        ClassMemberConstraints() = default;
        virtual ~ClassMemberConstraints() = default;

        void set_min_value(int min_value)
        {
            this->min_value = min_value;
        }

        auto get_min_value() const
        {
            return min_value;
        }

        void set_max_value(int max_value)
        {
            this->max_value = max_value;
        }

        auto get_max_value() const
        {
            return max_value;
        }

        void set_min_length(size_t min_length)
        {
            this->min_length = min_length;
        }

        auto get_min_length() const
        {
            return min_length;
        }

        void set_max_length(size_t max_length)
        {
            this->max_length = max_length;
        }

        auto get_max_length() const
        {
            return max_length;
        }

        void set_pattern(const std::string &pattern)
        {
            this->pattern = pattern;
        }

        auto get_pattern() const
        {
            return pattern;
        }
    };

    class ClassMemberConstraintException : public std::runtime_error
    {
    public:
        ClassMemberConstraintException(const std::string &msg) : std::runtime_error(msg)
        {
        }
    };

    class ValueTooLowException : public ClassMemberConstraintException
    {
    public:
        ValueTooLowException(const std::string &msg) : ClassMemberConstraintException(msg)
        {
        }
    };

    class ValueTooHighException : public ClassMemberConstraintException
    {
    public:
        ValueTooHighException(const std::string &msg) : ClassMemberConstraintException(msg)
        {
        }
    };

    class ValueTooShortException : public ClassMemberConstraintException
    {
    public:
        ValueTooShortException(const std::string &msg) : ClassMemberConstraintException(msg)
        {
        }
    };

    class ValueTooLongException : public ClassMemberConstraintException
    {
    public:
        ValueTooLongException(const std::string &msg) : ClassMemberConstraintException(msg)
        {
        }
    };

    class InvalidPatternException : public ClassMemberConstraintException
    {
    public:
        InvalidPatternException(const std::string &msg) : ClassMemberConstraintException(msg)
        {
        }
    };

    void CheckConstraint(const std::string &name, const ClassMemberConstraints &c, int64_t value)
    {
        if (c.get_min_value() != std::nullopt && value < *c.get_min_value())
        {
            throw ValueTooLowException("Value too low for " + name + " (" + std::to_string(value) + "<" + std::to_string(*c.get_min_value()) + ")");
        }

        if (c.get_max_value() != std::nullopt && value > *c.get_max_value())
        {
            throw ValueTooHighException("Value too high for " + name + " (" + std::to_string(value) + ">" + std::to_string(*c.get_max_value()) + ")");
        }
    }

    void CheckConstraint(const std::string &name, const ClassMemberConstraints &c, const std::string &value)
    {
        if (c.get_min_length() != std::nullopt && value.length() < *c.get_min_length())
        {
            throw ValueTooShortException("Value too short for " + name + " (" + std::to_string(value.length()) + "<" + std::to_string(*c.get_min_length()) + ")");
        }

        if (c.get_max_length() != std::nullopt && value.length() > *c.get_max_length())
        {
            throw ValueTooLongException("Value too long for " + name + " (" + std::to_string(value.length()) + ">" + std::to_string(*c.get_max_length()) + ")");
        }

        if (c.get_pattern() != std::nullopt)
        {
            std::smatch result;
            std::regex_search(value, result, std::regex(*c.get_pattern()));
            if (result.empty())
            {
                throw InvalidPatternException("Value doesn't match pattern for " + name + " (" + value + " != " + *c.get_pattern() + ")");
            }
        }
    }

    inline json get_untyped(const json &j, const char *property)
    {
        if (j.find(property) != j.end())
        {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json &j, std::string property)
    {
        return get_untyped(j, property.data());
    }

    template<typename T>
    inline std::shared_ptr<T> get_optional(const json &j, const char *property)
    {
        if (j.find(property) != j.end())
        {
            return j.at(property).get<std::shared_ptr<T>>();
        }
        return std::shared_ptr<T>();
    }

    template<typename T>
    inline std::shared_ptr<T> get_optional(const json &j, std::string property)
    {
        return get_optional<T>(j, property.data());
    }

    class Metadata
    {
    public:
        Metadata() = default;
        virtual ~Metadata() = default;

    private:
        std::shared_ptr<std::vector<std::string>> prop;

    public:
        std::shared_ptr<std::vector<std::string>> get_prop() const
        {
            return prop;
        }

        void set_prop(std::shared_ptr<std::vector<std::string>> value)
        {
            this->prop = value;
        }
    };

    class ColorsFlavor
    {
    public:
        ColorsFlavor() = default;
        virtual ~ColorsFlavor() = default;

    private:
        std::shared_ptr<std::vector<double>> color;
        std::shared_ptr<Metadata> metadata;

    public:
        std::shared_ptr<std::vector<double>> get_color() const
        {
            return color;
        }

        void set_color(std::shared_ptr<std::vector<double>> value)
        {
            this->color = value;
        }

        std::shared_ptr<Metadata> get_metadata() const
        {
            return metadata;
        }

        void set_metadata(std::shared_ptr<Metadata> value)
        {
            this->metadata = value;
        }
    };

    class Colors
    {
    public:
        Colors() = default;
        virtual ~Colors() = default;

    private:
        std::shared_ptr<std::vector<ColorsFlavor>> flavors;

    public:
        std::shared_ptr<std::vector<ColorsFlavor>> get_flavors() const
        {
            return flavors;
        }

        void set_flavors(std::shared_ptr<std::vector<ColorsFlavor>> value)
        {
            this->flavors = value;
        }
    };

    class Instructions
    {
    public:
        Instructions() = default;
        virtual ~Instructions() = default;

    private:
        std::shared_ptr<std::string> design_system_format;
        std::shared_ptr<std::vector<std::string>> metadata;

    public:
        std::shared_ptr<std::string> get_design_system_format() const
        {
            return design_system_format;
        }

        void set_design_system_format(std::shared_ptr<std::string> value)
        {
            this->design_system_format = value;
        }

        std::shared_ptr<std::vector<std::string>> get_metadata() const
        {
            return metadata;
        }

        void set_metadata(std::shared_ptr<std::vector<std::string>> value)
        {
            this->metadata = value;
        }
    };

    class Curve
    {
    public:
        Curve() = default;
        virtual ~Curve() = default;

    private:
        std::shared_ptr<std::string> curvetype;
        std::shared_ptr<std::vector<nlohmann::json>> points;

    public:
        std::shared_ptr<std::string> get_curvetype() const
        {
            return curvetype;
        }

        void set_curvetype(std::shared_ptr<std::string> value)
        {
            this->curvetype = value;
        }

        std::shared_ptr<std::vector<nlohmann::json>> get_points() const
        {
            return points;
        }

        void set_points(std::shared_ptr<std::vector<nlohmann::json>> value)
        {
            this->points = value;
        }
    };

    class MotionFlavor
    {
    public:
        MotionFlavor() = default;
        virtual ~MotionFlavor() = default;

    private:
        std::shared_ptr<Curve> curve;
        std::shared_ptr<int64_t> delay;
        std::shared_ptr<std::string> delayunits;
        std::shared_ptr<std::string> description;
        std::shared_ptr<double> duration;
        std::shared_ptr<std::string> durationunit;
        std::shared_ptr<std::string> name;
        std::shared_ptr<std::string> title;
        std::shared_ptr<std::string> transformation;

    public:
        std::shared_ptr<Curve> get_curve() const
        {
            return curve;
        }

        void set_curve(std::shared_ptr<Curve> value)
        {
            this->curve = value;
        }

        std::shared_ptr<int64_t> get_delay() const
        {
            return delay;
        }

        void set_delay(std::shared_ptr<int64_t> value)
        {
            this->delay = value;
        }

        std::shared_ptr<std::string> get_delayunits() const
        {
            return delayunits;
        }

        void set_delayunits(std::shared_ptr<std::string> value)
        {
            this->delayunits = value;
        }

        std::shared_ptr<std::string> get_description() const
        {
            return description;
        }

        void set_description(std::shared_ptr<std::string> value)
        {
            this->description = value;
        }

        std::shared_ptr<double> get_duration() const
        {
            return duration;
        }

        void set_duration(std::shared_ptr<double> value)
        {
            this->duration = value;
        }

        std::shared_ptr<std::string> get_durationunit() const
        {
            return durationunit;
        }

        void set_durationunit(std::shared_ptr<std::string> value)
        {
            this->durationunit = value;
        }

        std::shared_ptr<std::string> get_name() const
        {
            return name;
        }

        void set_name(std::shared_ptr<std::string> value)
        {
            this->name = value;
        }

        std::shared_ptr<std::string> get_title() const
        {
            return title;
        }

        void set_title(std::shared_ptr<std::string> value)
        {
            this->title = value;
        }

        std::shared_ptr<std::string> get_transformation() const
        {
            return transformation;
        }

        void set_transformation(std::shared_ptr<std::string> value)
        {
            this->transformation = value;
        }
    };

    class Motion
    {
    public:
        Motion() = default;
        virtual ~Motion() = default;

    private:
        std::shared_ptr<std::vector<MotionFlavor>> flavors;

    public:
        std::shared_ptr<std::vector<MotionFlavor>> get_flavors() const
        {
            return flavors;
        }

        void set_flavors(std::shared_ptr<std::vector<MotionFlavor>> value)
        {
            this->flavors = value;
        }
    };

    class TypographyFlavor
    {
    public:
        TypographyFlavor() :
            size_constraint(1, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {
        }

        virtual ~TypographyFlavor() = default;

    private:
        std::shared_ptr<std::string> alignment;
        std::shared_ptr<std::vector<double>> color;
        std::shared_ptr<std::string> font;
        std::shared_ptr<std::string> name;
        std::shared_ptr<std::string> rem;
        std::shared_ptr<double> size;
        ClassMemberConstraints size_constraint;

    public:
        std::shared_ptr<std::string> get_alignment() const
        {
            return alignment;
        }

        void set_alignment(std::shared_ptr<std::string> value)
        {
            this->alignment = value;
        }

        std::shared_ptr<std::vector<double>> get_color() const
        {
            return color;
        }

        void set_color(std::shared_ptr<std::vector<double>> value)
        {
            this->color = value;
        }

        std::shared_ptr<std::string> get_font() const
        {
            return font;
        }

        void set_font(std::shared_ptr<std::string> value)
        {
            this->font = value;
        }

        std::shared_ptr<std::string> get_name() const
        {
            return name;
        }

        void set_name(std::shared_ptr<std::string> value)
        {
            this->name = value;
        }

        std::shared_ptr<std::string> get_rem() const
        {
            return rem;
        }

        void set_rem(std::shared_ptr<std::string> value)
        {
            this->rem = value;
        }

        std::shared_ptr<double> get_size() const
        {
            return size;
        }

        void set_size(std::shared_ptr<double> value)
        {
            if (value)
                CheckConstraint("size", size_constraint, *value);
            this->size = value;
        }
    };

    class Typography
    {
    public:
        Typography() = default;
        virtual ~Typography() = default;

    private:
        std::shared_ptr<std::vector<TypographyFlavor>> flavors;

    public:
        std::shared_ptr<std::vector<TypographyFlavor>> get_flavors() const
        {
            return flavors;
        }

        void set_flavors(std::shared_ptr<std::vector<TypographyFlavor>> value)
        {
            this->flavors = value;
        }
    };

    class DesignSystem
    {
    public:
        DesignSystem() = default;
        virtual ~DesignSystem() = default;

    private:
        std::shared_ptr<Colors> colors;
        std::shared_ptr<std::map<std::string, nlohmann::json>> components;
        std::shared_ptr<Instructions> instructions;
        std::shared_ptr<Motion> motion;
        std::shared_ptr<Typography> typography;

    public:
        std::shared_ptr<Colors> get_colors() const
        {
            return colors;
        }

        void set_colors(std::shared_ptr<Colors> value)
        {
            this->colors = value;
        }

        std::shared_ptr<std::map<std::string, nlohmann::json>> get_components() const
        {
            return components;
        }

        void set_components(std::shared_ptr<std::map<std::string, nlohmann::json>> value)
        {
            this->components = value;
        }

        std::shared_ptr<Instructions> get_instructions() const
        {
            return instructions;
        }

        void set_instructions(std::shared_ptr<Instructions> value)
        {
            this->instructions = value;
        }

        std::shared_ptr<Motion> get_motion() const
        {
            return motion;
        }

        void set_motion(std::shared_ptr<Motion> value)
        {
            this->motion = value;
        }

        std::shared_ptr<Typography> get_typography() const
        {
            return typography;
        }

        void set_typography(std::shared_ptr<Typography> value)
        {
            this->typography = value;
        }
    };
}

namespace nlohmann
{
    void from_json(const json &j, Youi::Metadata &x);
    void to_json(json &j, const Youi::Metadata &x);

    void from_json(const json &j, Youi::ColorsFlavor &x);
    void to_json(json &j, const Youi::ColorsFlavor &x);

    void from_json(const json &j, Youi::Colors &x);
    void to_json(json &j, const Youi::Colors &x);

    void from_json(const json &j, Youi::Instructions &x);
    void to_json(json &j, const Youi::Instructions &x);

    void from_json(const json &j, Youi::Curve &x);
    void to_json(json &j, const Youi::Curve &x);

    void from_json(const json &j, Youi::MotionFlavor &x);
    void to_json(json &j, const Youi::MotionFlavor &x);

    void from_json(const json &j, Youi::Motion &x);
    void to_json(json &j, const Youi::Motion &x);

    void from_json(const json &j, Youi::TypographyFlavor &x);
    void to_json(json &j, const Youi::TypographyFlavor &x);

    void from_json(const json &j, Youi::Typography &x);
    void to_json(json &j, const Youi::Typography &x);

    void from_json(const json &j, Youi::DesignSystem &x);
    void to_json(json &j, const Youi::DesignSystem &x);

    inline void from_json(const json &j, Youi::Metadata &x)
    {
        x.set_prop(Youi::get_optional<std::vector<std::string>>(j, "prop"));
    }

    inline void to_json(json &j, const Youi::Metadata &x)
    {
        j = json::object();
        j["prop"] = x.get_prop();
    }

    inline void from_json(const json &j, Youi::ColorsFlavor &x)
    {
        x.set_color(Youi::get_optional<std::vector<double>>(j, "color"));
        x.set_metadata(Youi::get_optional<Youi::Metadata>(j, "metadata"));
    }

    inline void to_json(json &j, const Youi::ColorsFlavor &x)
    {
        j = json::object();
        j["color"] = x.get_color();
        j["metadata"] = x.get_metadata();
    }

    inline void from_json(const json &j, Youi::Colors &x)
    {
        x.set_flavors(Youi::get_optional<std::vector<Youi::ColorsFlavor>>(j, "flavors"));
    }

    inline void to_json(json &j, const Youi::Colors &x)
    {
        j = json::object();
        j["flavors"] = x.get_flavors();
    }

    inline void from_json(const json &j, Youi::Instructions &x)
    {
        x.set_design_system_format(Youi::get_optional<std::string>(j, "designSystemFormat"));
        x.set_metadata(Youi::get_optional<std::vector<std::string>>(j, "metadata"));
    }

    inline void to_json(json &j, const Youi::Instructions &x)
    {
        j = json::object();
        j["designSystemFormat"] = x.get_design_system_format();
        j["metadata"] = x.get_metadata();
    }

    inline void from_json(const json &j, Youi::Curve &x)
    {
        x.set_curvetype(Youi::get_optional<std::string>(j, "curvetype"));
        x.set_points(Youi::get_optional<std::vector<json>>(j, "points"));
    }

    inline void to_json(json &j, const Youi::Curve &x)
    {
        j = json::object();
        j["curvetype"] = x.get_curvetype();
        j["points"] = x.get_points();
    }

    inline void from_json(const json &j, Youi::MotionFlavor &x)
    {
        x.set_curve(Youi::get_optional<Youi::Curve>(j, "curve"));
        x.set_delay(Youi::get_optional<int64_t>(j, "delay"));
        x.set_delayunits(Youi::get_optional<std::string>(j, "delayunits"));
        x.set_description(Youi::get_optional<std::string>(j, "description"));
        x.set_duration(Youi::get_optional<double>(j, "duration"));
        x.set_durationunit(Youi::get_optional<std::string>(j, "durationunit"));
        x.set_name(Youi::get_optional<std::string>(j, "name"));
        x.set_title(Youi::get_optional<std::string>(j, "title"));
        x.set_transformation(Youi::get_optional<std::string>(j, "transformation"));
    }

    inline void to_json(json &j, const Youi::MotionFlavor &x)
    {
        j = json::object();
        j["curve"] = x.get_curve();
        j["delay"] = x.get_delay();
        j["delayunits"] = x.get_delayunits();
        j["description"] = x.get_description();
        j["duration"] = x.get_duration();
        j["durationunit"] = x.get_durationunit();
        j["name"] = x.get_name();
        j["title"] = x.get_title();
        j["transformation"] = x.get_transformation();
    }

    inline void from_json(const json &j, Youi::Motion &x)
    {
        x.set_flavors(Youi::get_optional<std::vector<Youi::MotionFlavor>>(j, "flavors"));
    }

    inline void to_json(json &j, const Youi::Motion &x)
    {
        j = json::object();
        j["flavors"] = x.get_flavors();
    }

    inline void from_json(const json &j, Youi::TypographyFlavor &x)
    {
        x.set_alignment(Youi::get_optional<std::string>(j, "alignment"));
        x.set_color(Youi::get_optional<std::vector<double>>(j, "color"));
        x.set_font(Youi::get_optional<std::string>(j, "font"));
        x.set_name(Youi::get_optional<std::string>(j, "name"));
        x.set_rem(Youi::get_optional<std::string>(j, "rem"));
        x.set_size(Youi::get_optional<double>(j, "size"));
    }

    inline void to_json(json &j, const Youi::TypographyFlavor &x)
    {
        j = json::object();
        j["alignment"] = x.get_alignment();
        j["color"] = x.get_color();
        j["font"] = x.get_font();
        j["name"] = x.get_name();
        j["rem"] = x.get_rem();
        j["size"] = x.get_size();
    }

    inline void from_json(const json &j, Youi::Typography &x)
    {
        x.set_flavors(Youi::get_optional<std::vector<Youi::TypographyFlavor>>(j, "flavors"));
    }

    inline void to_json(json &j, const Youi::Typography &x)
    {
        j = json::object();
        j["flavors"] = x.get_flavors();
    }

    inline void from_json(const json &j, Youi::DesignSystem &x)
    {
        x.set_colors(Youi::get_optional<Youi::Colors>(j, "colors"));
        x.set_components(Youi::get_optional<std::map<std::string, json>>(j, "components"));
        x.set_instructions(Youi::get_optional<Youi::Instructions>(j, "instructions"));
        x.set_motion(Youi::get_optional<Youi::Motion>(j, "motion"));
        x.set_typography(Youi::get_optional<Youi::Typography>(j, "typography"));
    }

    inline void to_json(json &j, const Youi::DesignSystem &x)
    {
        j = json::object();
        j["colors"] = x.get_colors();
        j["components"] = x.get_components();
        j["instructions"] = x.get_instructions();
        j["motion"] = x.get_motion();
        j["typography"] = x.get_typography();
    }
}
