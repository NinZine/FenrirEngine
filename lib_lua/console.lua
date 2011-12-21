local p = {
	showing = false,
	current_string = "",
	current_cursor = "",
	cursor = "|",
	last_blink = 0,
	blink_frequency = 500,
	up_count = 0,
	scroll_line = 0,
	prev = {}
}

function p.key_down(key)
	if key.sym == event.KEY_ESCAPE then
		p.toggle()
	elseif key.sym == event.KEY_BACKSPACE then
		p.current_string = string.sub(p.current_string, 1,
			string.len(p.current_string)-1)
	elseif key.sym == event.KEY_RETURN then
		sys.exec(p.current_string)
		table.insert(p.prev, p.current_string)
		p.current_string = ""
		p.up_count = 0
	elseif key.sym == event.KEY_UP then
		if p.up_count < #p.prev then
			p.up_count = p.up_count + 1
			p.current_string = p.prev[p.up_count]
		end
	elseif key.sym == event.KEY_DOWN then
		if p.up_count > 1 then
			p.up_count = p.up_count - 1
			p.current_string = p.prev[p.up_count]
		elseif p.up_count == 1 then
			p.up_count = p.up_count - 1
			p.current_string = ""
		end
	elseif key.sym == event.KEY_LEFT then
		p.scrolling = 1
	elseif key.sym == event.KEY_RIGHT then
		p.scrolling = -1
	else
		p.current_string = p.current_string .. key.c
	end
end

function p.key_up(key)
	if key.sym == event.KEY_LEFT or key.sym == event.KEY_RIGHT then
		p.scrolling = 0
	end
end

function p.scroll()
	if p.scrolling == -1 and p.scroll_line > 0 then
		p.scroll_line = p.scroll_line - 1
	end

	if p.scrolling == 1 and p.scroll_line < log.lines() - 1 then
		p.scroll_line = p.scroll_line + 1
	end
end

function p.toggle()
	if p.showing == true then
		p.showing = false
	else
		p.showing = true
	end
end

function p.render()
	if p.showing == false then return end

	if p.last_blink+p.blink_frequency < event.time() then
		p.last_blink = event.time()
		if p.current_cursor == p.cursor then
			p.current_cursor = ""
		else
			p.current_cursor = p.cursor
		end
	end

	p.scroll()

    render.disable_depth();
	render.push_matrix()
    render.load_identity()
    
    render.matrix_mode(1)
    render.push_matrix()
    render.setup_orthogonal_view(480, 320)
    
    render.color(1, 1, 1)
	render.push_matrix()
	render.translate(0, 79, 0)
	render.scale(480, 160, 1)
	render.render_quad(1)
	render.pop_matrix()
    render.color(0.2, 0.2, 0.6)
	render.translate(0, 80, 0)
	render.scale(480, 160, 1)
	render.render_quad(1)

	hud.draw_text("> " .. p.current_string .. p.current_cursor, -230, 10)
	p.render_log()

    render.matrix_mode(1)
    render.pop_matrix()

    render.matrix_mode(0)
	render.pop_matrix()
    render.enable_depth()

end

function p.render_log()
	local y = 20
	local b = log.lines() - 1 - p.scroll_line
	local e = b - 13

	if e < 0 then e = 0 end

	for i = b, e, -1 do
		hud.draw_text(log.get_line(i), -230, y)

		y = y + 10
	end
end

console = p
return console

