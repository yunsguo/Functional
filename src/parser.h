/*
*   parsing.h:
*	previously Hutton.h
*   A port of Hutton's parsing library
*	included in solution directory
*   Language: C++, Visual Studio 2017
*   Platform: Windows 10 Pro
*   Application: recreational
*   Author: Yunsheng Guo, yguo125@syr.edu
*/


/*
*
*   Package Operations:
*	utilize monad operations to create a general parsing library
*	by combining Parser monad
*
*   Public Interface:
*	check Hutton's parsing library for all methods
*
*   Build Process:
*   requires prelude.h
*
*   Maintenance History:
*   July 18
*   First draft
*   late July
*   decided that in order to achieve monadic sequence, reverse apply function order has to be achieved
*	August 20
*	refactor according to other dependents
*	August 25
*	final review for publish
*
*
*/

#pragma once
#ifndef _PARSER_
#define _PARSER_

#include "prelude.h"

namespace fcl
{

	template<typename a>
	using Reaction = Maybe<Pair<a, std::string>>;

	template<typename a>
	struct Parser;

	template<typename a>
	Reaction<a> parse(const Parser<a>&, const std::string&);

	template<typename a>
	struct Parser
	{
		template<typename l, typename = std::enable_if_t<std::is_convertible<l, Function<Reaction<a>, std::string>>::value>>
		Parser(l lambda) :P(lambda) {}
		friend Reaction<a> parse<>(const Parser<a>&, const std::string&);
	private:
		Function<Reaction<a>, std::string> P;
	};

	template<typename a>
	Reaction<a> parse(const Parser<a>& p, const std::string& str) { return p.P(str); }

	template<typename a>
	Reaction<a> failure(std::string inp) 
	{ 
		const static Nothing nothing = Nothing();
		return nothing;
	}

	template<>
	struct Functor<Parser>
	{
		using pertain = std::true_type;

	private:

		template<typename f, typename = std::enable_if_t<is_function<f>::value>>
		static Reaction<applied_type<f>> fmap_impl(f f_, Parser<head_parameter<f>> pa, std::string inp)
		{
			auto r = parse(pa, inp);
			if (isNothing(r)) return Nothing();
			auto pair = fromJust(r);
			return std::make_pair<applied_type<f>, std::string>(f_ << pair.first, pair.second);
		}

	public:

		template<typename f, typename = std::enable_if_t<is_function<f>::value>>
		static Parser<applied_type<f>> fmap(const f& f_, const Parser<head_parameter<f>>& pa)
		{
			const static auto Ffmap = Function<Reaction<applied_type<f>>, f, Parser<head_parameter<f>>, std::string>(fmap_impl<f>);
			return Ffmap << f_ << pa;
		}
	};

	template<>
	struct Alternative<Parser>
	{
		using pertain = std::true_type;

	private:
		template<typename a>
		static Reaction<a> alter_impl(Parser<a> p, Parser<a> q, std::string inp)
		{
			auto r = parse(p, inp);
			if (isJust(r)) return r;
			return parse(q, inp);
		}

	public:

		template<typename a>
		static Parser<a> empty() { return failure<a>; }

		template<typename a>
		static Parser<a> alter(const Parser<a>& p, const Parser<a>& q)
		{
			const static auto Falter = Function<Reaction<a>, Parser<a>, Parser<a>, std::string>(alter_impl<a>);
			return Falter << p << q;
		}
	};

	template<>
	struct Monad<Parser>
	{
		using pertain = std::true_type;

	private:

		template<typename a>
		static Reaction<a> pure_impl(a value, std::string inp)
		{
			return std::make_pair<a, std::string>(std::move(value), std::move(inp));
		}

		template<typename f, typename = std::enable_if_t<is_function<f>::value>>
		static Reaction<monadic_applied_type<f>> seq_impl(Parser<last_parameter<f>> pa, Parser<f> pf, std::string inp)
		{
			auto ra = parse(pa, inp);
			if (isNothing(ra)) return Nothing();
			auto paira = fromJust(ra);
			auto rf = parse(pf, paira.second);
			if (isNothing(rf)) return Nothing();
			auto pairf = fromJust(rf);
			return std::make_pair<monadic_applied_type<f>, std::string>(paira.first >>= pairf.first, std::move(pairf.second));
		}

		template<typename a, typename b>
		static Reaction<b> compose_impl(Parser<a> p, Parser<b> q, std::string inp)
		{
			auto r = parse(p, inp);
			if (isNothing(r)) return Nothing();
			auto pair = fromJust(r);
			return parse(q, pair.second);
		}

	public:

		template<typename a>
		static Parser<a> pure(const a& value)
		{
			const static auto Fpure = Function<Reaction<a>, a, std::string>(pure_impl<a>);
			return Fpure << value;
		}

		template<typename f, typename = std::enable_if_t<is_function<f>::value>>
		static Parser<monadic_applied_type<f>> sequence(const Parser<last_parameter<f>>& pa, const Parser<f>& pf)
		{
			const static auto Fseq = Function<Reaction<monadic_applied_type<f>>, Parser<last_parameter<f>>, Parser<f>, std::string>(seq_impl<f>);
			return Fseq << pa << pf;
		}

		template<typename a, typename b>
		static Parser<b> compose(const Parser<a>& p, const Parser<b>& q)
		{
			const static auto Fcompose = Function<Reaction<b>, Parser<a>, Parser<b>, std::string>(compose_impl<a, b>);
			return Fcompose << p << q;
		}
	};

	Reaction<char> item(std::string inp);

	Parser<char> sat(Function<bool, char> p);

	Reaction<char> digit(std::string inp);

	Reaction<char> digit19(std::string inp);

	Reaction<char> hexdecimal(std::string inp);

	Reaction<char> lower(std::string inp);

	Reaction<char> upper(std::string inp);

	Reaction<char> letter(std::string inp);

	Reaction<char> alphanumeric(std::string inp);

	Parser<char> character(char c);

	std::string one_char_str(char c);

	Parser<std::string> string(std::string str);

	Reaction<std::string> ident(std::string inp);

	Reaction<int> nat(std::string inp);

	Reaction<int> inte(std::string inp);

	Reaction<NA> space(std::string inp);

	Reaction<std::string> identifier(std::string inp);

	Reaction<int> natural(std::string inp);

	Reaction<int> integer(std::string inp);

	Parser<std::string> symbol(std::string str);

	std::string cons_str(char c, std::string str);

	Pair<char, std::string> uncons_str(std::string str);

	std::string concat_str(std::string, std::string);

	Parser<std::string> maybe_one(Parser<char> p);

	Parser<std::string> any(Parser<char> p);

	Parser<std::string> some(Parser<char> p);

	template<typename a>
	Parser<List<a>> any(Parser<a> p);

	template<typename a>
	Parser<List<a>> some(Parser<a> p);

	template<typename a>
	Parser<a> token(Parser<a> p)
	{
		const static auto Fid = Monad<Parser>::pure<Function<a, a>>(id<a>);
		const static auto Fspace = Parser<NA>(space);
		return
			Fspace >>
			p >>=
			Fspace >>
			Fid;
	}

#ifdef PARSER_CPP

	int add(int a, int b) { return a + b; }

	int mul(int a, int b) { return a * b; }

	Reaction<int> expr(std::string inp);

	Reaction<int> term(std::string inp);

	Reaction<int> factor(std::string inp);

	int digitToInt(char c) { return c - '0'; }

	int eval(std::string inp);

#endif

	//implenmentation
	template<typename a>
	inline Parser<List<a>> any(Parser<a> p) 
	{
		const static Function<Reaction<List<a>>, Parser<a>, std::string> any_impl =
			[](Parser<a> p, std::string inp) ->Reaction<List<a>> 
		{
			auto reaction = parse(p, inp);
			if (isNothing(reaction))
				return std::make_pair<List<a>, std::string>(List<a>(), std::move(inp));
			auto pair = fromJust(reaction);
			List<a> ans;
			std::string inpn;
			while (true)
			{
				ans.push_back(std::move(pair.first));
				inpn = std::move(pair.second);
				reaction = parse(p, inpn);
				if (isNothing(reaction)) return std::make_pair<List<a>, std::string>(std::move(ans), std::move(inpn));
				pair = fromJust(reaction);
			}
		};
		return any_impl << p;
	}

	template<typename a>
	inline Parser<List<a>> some(Parser<a> p)
	{
		const static auto Mcons = Monad<Parser>::pure(Function<List<a>, a, List<a>>(cons<a>));
		return
			p >>=
			any<a>(p) >>=
			Mcons;
	}
}

#endif